// Importacion de librerias
#include "mbed.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Declaracion de pines para los leds
DigitalOut led1(LED1);
DigitalOut led2(LED2);

#define BUFF_LEN    32
#define MSG_LEN     64
#define DATA_LEN    MSG_LEN - 2

BufferedSerial      serial(STDIO_UART_TX, STDIO_UART_RX);
Thread              thread1;
Thread              thread2;
EventQueue          eventQueue;
Mutex               mutex;
ConditionVariable   cond(mutex);
char                recvBuff[BUFF_LEN] = { 0 };
size_t              recvLen;
char                message[MSG_LEN] = { 0 };

// The following variable is protected by locking the mutex
char                data[DATA_LEN] = { 0 };

unsigned short depth;
unsigned short amplitudMaxima;
unsigned short simetria;
unsigned short desvio;
char                clase[1] = { 0 };




char clasificador(unsigned short depth, unsigned short amplitudMaxima, unsigned short simetria, unsigned short desvio){
         
   switch (depth){

   case 2:
        if (amplitudMaxima <= 37){
            if (amplitudMaxima <= 19){
                clase[0] = '1';
            }
            if (amplitudMaxima > 19){
                clase[0] = '1';    
            }
        }
        if (amplitudMaxima > 37){
            if (simetria <= 50){
                clase[0] = '0';
            }
            if (simetria > 50){
                clase[0] = '2';    
            }
        }
    break;
   case 3:       
    break;   
   case 4:       
    break;  
   case 5:       
    break;
   default:
    clase[0] = 'E';
   }

   return clase[0];
}


void onSerialSend()
{
    mutex.lock();

    while (1) {
      
        // Wait for a condition to change
        cond.wait();
 
        // Now it is safe to access data in this thread 
        depth = (data[0]-48);
        amplitudMaxima = (data[8]-48)*10 + (data[9]-48);
        simetria = (data[11]-48)*10 + (data[12]-48);
        desvio = (data[14]-48)*10 + (data[15]-48);
 
        //printf("amplitudMaxima  %u\n", (unsigned int)amplitudMaxima);  
        //printf("simetria  %u\n", (unsigned int)simetria);  
        //printf("desvio  %u\n", (unsigned int)desvio);  
        //serial.write(amplitudMaxima, sizeof(amplitudMaxima));

        clase[0] = clasificador(depth, amplitudMaxima, simetria, desvio);

        serial.write(clase, sizeof(clase)); 
        memset(data, 0, DATA_LEN);  // empty data to make space for new data
        memset(clase, ' ', sizeof(clase));  // empty data
    }
}

void onSerialReceived(void)
{
    while (serial.readable()) {
        // Read serial
        recvLen = serial.read(recvBuff, BUFF_LEN);

        if ((strlen(message) + recvLen) > MSG_LEN) {
            // too much data -> something went wrong
            memset(message, 0, MSG_LEN);
            break;
        }

        strcat(message, recvBuff);          // append received chars to message

        if (message[0] != 's') {
            // garbage message received
            memset(message, 0, MSG_LEN);
            break;
        }

        if (message[strlen(message) - 1] == '#') {
            // message complete
            mutex.lock();

            // copy the chars from the message to the data storage
            strcat(data, &message[1]);      // omit first char (which is 's')
            data[strlen(data) - 1] = '\0';  // delete last char (which is '#')
            memset(message, 0, MSG_LEN);

            // Signal for other threads that data has been received
            cond.notify_all();
            mutex.unlock();
            break;
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