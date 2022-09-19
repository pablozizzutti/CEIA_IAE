// Importacion de librerias
#include "mbed.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Definicion de constantes
#define BUFF_LEN    2
#define MSG_LEN     3

#define UMBRAL_APERTURA 3500
#define UMBRAL_CIERRE 300
#define HOLD 0.3
#define SAMPLERATE 22050

// Declaracion de pines para los leds
DigitalOut led1(LED1);
DigitalOut led2(LED2);

// Declaracion de variables
BufferedSerial      serial(STDIO_UART_TX, STDIO_UART_RX);
Thread              thread1;
Thread              thread2;
EventQueue          eventQueue;
Mutex               mutex;
ConditionVariable   cond(mutex);
char                recvBuff[BUFF_LEN] = { 0 };
size_t              recvLen;
char                sendBuff[MSG_LEN] = { 0 };
char                message[MSG_LEN] = { 0 };

typedef struct
{
  uint16_t open;
  uint16_t close;
  uint32_t samplesHold;
  uint32_t openCounter;
  bool isOpen;
} Config;

Config config;

// Funciones auxiliares
uint16_t toUint16(int value)
{
  return value + 32768;
}

bool puertaDeRuido(uint16_t sample, Config *config)
{
  if (sample >= config->open)
  {
    config->isOpen = true;
    config->openCounter = 0;
  }
  else if (config->isOpen)
  {
    if (sample < config->close && config->openCounter >= config->samplesHold)
      config->isOpen = false;
    else
      config->openCounter++;
  }

  return config->isOpen;
}
 

void onSerialSend()
{
    mutex.lock();

    while (1) {
        // Wait for a condition to change
        cond.wait();
 
        uint16_t sample = ((uint16_t)message[1] << 8) | message[0];
 
        bool isOpen = puertaDeRuido(sample, (Config *)&config);
        
        uint16_t result = isOpen ? sample : toUint16(0);
        
        serial.write(&result, sizeof(uint16_t));  
        
        // empty data to make space for new data
        memset(message, 0, BUFF_LEN);
    }
}

void onSerialReceived(void)
{
    while (serial.readable()) {
        // Read serial
        recvLen = serial.read(recvBuff, BUFF_LEN);   

        printf("recvLen  %u\n", (unsigned int)recvLen);   
        
        // Append received chars to message
        strcat(message, recvBuff);    
 
        // cuando lleguen 2 bytes
        if (strlen(message) == 2){
            // message complete
            mutex.lock();
            //strcat(data, message);

            // Signal for other threads that data has been received        
            cond.notify_all();
            mutex.unlock();  
        }      
    }
    memset(recvBuff, 0, BUFF_LEN);
}

// Evento ante la llegada de caracteres por el puerto UART
void onSigio(void)
{   
    eventQueue.call(onSerialReceived);
}

int main()
{
    // Set desired properties (115200-8-N-1)
    serial.set_baud(115200);
    serial.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );

    // Inicializacion de la estructura config
    config.open = toUint16(UMBRAL_APERTURA);
    config.close = toUint16(UMBRAL_CIERRE);
    config.samplesHold = HOLD * SAMPLERATE;
    config.openCounter = 0;
    config.isOpen = false;

    // declaracion de los threads
    thread1.start(onSerialSend);
    thread2.start(callback(&eventQueue, &EventQueue::dispatch_forever));
    serial.sigio(callback(onSigio));

    // Led Heartbeat
    while (1) {
        led1 = !led1;       
        ThisThread::sleep_for(500ms);
    }
}