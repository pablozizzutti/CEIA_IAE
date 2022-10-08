// Importacion de librerias
#include "mbed.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Declaracion de pines para los leds
DigitalOut led1(LED1);
DigitalOut led2(LED2);

// Declaracion de define
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

unsigned short depth;
unsigned short duracion;
unsigned short crucesPorCero;
unsigned short amplitudMaxima;
unsigned short simetria;
unsigned short desvio;
char clase[1] = { 0 };

// Variable protegida bloqueda por mutex
char                data[DATA_LEN] = { 0 };


/*

    Funcion que lee los valores de entrada y realiza la clasificacion
    
*/
char clasificador(unsigned short depth, unsigned short duracion, 
                  unsigned short crucesPorCero, unsigned short amplitudMaxima, 
                  unsigned short simetria, unsigned short desvio){
         
    switch (depth){
    /*
    |--- Maximo <= 37
    |   |--- Maximo <= 19
    |   |   |--- class: 1
    |   |--- Maximo >  19
    |   |   |--- class: 1
    |--- Maximo >  37
    |   |--- Simetria <= 50
    |   |   |--- class: 0
    |   |--- Simetria >  50
    |   |   |--- class: 2
    */
    case 2:
        if (amplitudMaxima <= 37){
            if (amplitudMaxima <= 19){
                clase[0] = '1';
            }
            else {
                clase[0] = '1';    
            }
        }
        else {
            if (simetria <= 50){
                clase[0] = '0';
            }
            else {
                clase[0] = '2';    
            }
        }
        break;
    /*
    |--- Maximo <= 37
    |   |--- Maximo <= 19
    |   |   |--- Maximo <= 4
    |   |   |   |--- class: 1
    |   |   |--- Maximo >  4
    |   |   |   |--- class: 1
    |   |--- Maximo >  19
    |   |   |--- Simetria <= 41
    |   |   |   |--- class: 1
    |   |   |--- Simetria >  41
    |   |   |   |--- class: 1
    |--- Maximo >  37
    |   |--- Simetria <= 50
    |   |   |--- Cruces <= 4.50
    |   |   |   |--- class: 0
    |   |   |--- Cruces >  4.50
    |   |   |   |--- class: 2
    |   |--- Simetria >  50
    |   |   |--- Simetria <= 84
    |   |   |   |--- class: 2
    |   |   |--- Simetria >  84
    |   |   |   |--- class: 0
    */
    case 3:      
        if (amplitudMaxima <= 37){
            if (amplitudMaxima <= 19){                
                clase[0] = '1';
            }
            else {
                clase[0] = '1';    
            }
        }
        else {
            if (simetria <= 50){
                if (crucesPorCero <= 45){                
                    clase[0] = '0';
                }
                else {
                    clase[0] = '2';    
                }
            }
            else {
                if (simetria <= 84){                
                    clase[0] = '2';
                }
                else {
                    clase[0] = '0';    
                }                  
            }
        }
        break;   
    /*
    |--- Maximo <= 37
    |   |--- Maximo <= 19
    |   |   |--- Maximo <= 4
    |   |   |   |--- Desvio <= 1
    |   |   |   |   |--- class: 1
    |   |   |   |--- Desvio >  1
    |   |   |   |   |--- class: 0
    |   |   |--- Maximo >  4
    |   |   |   |--- Simetria <= 25
    |   |   |   |   |--- class: 1
    |   |   |   |--- Simetria >  25
    |   |   |   |   |--- class: 1
    |   |--- Maximo >  19
    |   |   |--- Simetria <= 41
    |   |   |   |--- Duracion <= 24
    |   |   |   |   |--- class: 1
    |   |   |   |--- Duracion >  24
    |   |   |   |   |--- class: 1
    |   |   |--- Simetria >  41
    |   |   |   |--- Desvio <= 3
    |   |   |   |   |--- class: 1
    |   |   |   |--- Desvio >  3
    |   |   |   |   |--- class: 1
    |--- Maximo >  37
    |   |--- Simetria <= 50
    |   |   |--- Cruces <= 45
    |   |   |   |--- Maximo <= 42
    |   |   |   |   |--- class: 1
    |   |   |   |--- Maximo >  42
    |   |   |   |   |--- class: 0
    |   |   |--- Cruces >  45
    |   |   |   |--- Desvio <= 8
    |   |   |   |   |--- class: 2
    |   |   |   |--- Desvio >  8
    |   |   |   |   |--- class: 0
    |   |--- Simetria >  50
    |   |   |--- Simetria <= 84
    |   |   |   |--- Maximo <= 46
    |   |   |   |   |--- class: 2
    |   |   |   |--- Maximo >  46
    |   |   |   |   |--- class: 2
    |   |   |--- Simetria >  84
    |   |   |   |--- Maximo <= 60
    |   |   |   |   |--- class: 1
    |   |   |   |--- Maximo >  60
    |   |   |   |   |--- class: 0
    */
    case 4:  
        if (amplitudMaxima <= 37){
            if (amplitudMaxima <= 19){                
                if (amplitudMaxima <= 4){    
                    if (desvio <= 1){                                
                        clase[0] = '1';    
                    }
                    else {
                        clase[0] = '0';        
                    }     
                }
                else {
                    if (simetria <= 25){                                
                        clase[0] = '1';    
                    }
                    else {
                        clase[0] = '1';        
                    }  
                }   
            }
            else {
                if (simetria <= 41){    
                    if (duracion <= 24){                                
                        clase[0] = '1';    
                    }
                    else {
                        clase[0] = '1';        
                    }     
                }
                else {
                    if (desvio <= 3){                                
                        clase[0] = '1';    
                    }
                    else {
                        clase[0] = '1';        
                    }  
                }  
            }
        }
        else {
            if (simetria <= 50){                
                if (crucesPorCero <= 45){    
                    if (amplitudMaxima <= 42){                                
                        clase[0] = '1';    
                    }
                    else {
                        clase[0] = '0';        
                    }     
                }
                else {
                    if (desvio <= 8){                                
                        clase[0] = '2';    
                    }
                    else {
                        clase[0] = '0';        
                    }  
                }   
            }
            else {
                if (simetria <= 84){    
                    if (amplitudMaxima <= 46){                                
                        clase[0] = '2';    
                    }
                    else {
                        clase[0] = '2';        
                    }     
                }
                else {
                    if (amplitudMaxima <= 60){                                
                        clase[0] = '1';    
                    }
                    else {
                        clase[0] = '0';        
                    }  
                }  
            }
        }   
        break;     
    // En el caso que el valor de depth sera diferente a los anteriores
    // devuelve el caracter 'E' que significa error
    default:
        clase[0] = 'E';
    }
   
    // devuelve el caracter para enviarlo por uart
    return clase[0];
}


/*

    Funcion para el envio de la clase por el puerto uart

*/
void onSerialSend()
{
    mutex.lock();

    while (1) {
      
        // Espera al cambio de condicion
        cond.wait();
 
        // Ahora es seguro acceder a los datos del thread
        // Los datos se envian con el siguiente formato:
        //
        // sd,dd,ccc,aaa,sds,ddd#
        //
        // s: caracter de start
        // d: valor del depth (profundidad del modelo)
        // ddd: duracion (s)
        // ccc: Cruces por cero
        // aaa: Amplitud máxima
        // sss: Simetria
        // ddd: Desvio
        // #: caracter de fin de dato
        // ,: caracter separador
        // 
        // los datos con punto flotante se tranforman en enteros en origin y se transmiten
        // asi al embebido para realizar la comparacion.

        // Ejemplo de dato:
        //
        //['Duración (s)', 'Cruces por cero', 'Amplitud máxima', 'Simetria', 'Desvio']
        //[0.3473469387755102, 7, 0.39420758690145574, 0.5745041270194682, 0.046092456313828016]
        // 
        // Valor transmitido al embebido:
        //
        // s2,034,070,039,057,004#

        depth = (data[0]-48);
        duracion = (data[2]-48)*100 + (data[3]-48)*10 + (data[4]-48);
        crucesPorCero = (data[6]-48)*100 + (data[7]-48)*10 + (data[8]-48);
        amplitudMaxima = (data[10]-48)*100 + (data[11]-48)*10 + (data[12]-48);
        simetria = (data[14]-48)*100 + (data[15]-48)*10 + (data[16]-48);
        desvio = (data[18]-48)*100 + (data[19]-48)*10 + (data[20]-48);
 
        // segun los datos de entrada realiza la clasificacion
        clase[0] = clasificador(depth, duracion, crucesPorCero, amplitudMaxima, simetria, desvio);

        // envia la clase por uart
        serial.write(clase, sizeof(clase)); 

        // borra los buffers
        memset(data, 0, DATA_LEN);  
        memset(clase, ' ', sizeof(clase));  
    }
}

/*

    Funcion para recibir los datos por el puerto UART

*/

void onSerialReceived(void)
{
    while (serial.readable()) {
        
        // lee los caracteres leidos
        recvLen = serial.read(recvBuff, BUFF_LEN);

        // Si la cantidad de datos en mayor a MSG_LEN, hubo un error, descarta el mensaje completo
        if ((strlen(message) + recvLen) > MSG_LEN) {
            memset(message, 0, MSG_LEN);
            break;
        }

        // Hace un concatenamiento de los datos recibidos al buffer del mensaje
        strcat(message, recvBuff);        

        // Si el mensaje no empieza con el caracter 's', hubo un error
        if (message[0] != 's') {
            memset(message, 0, MSG_LEN);
            break;
        }
        
        // Si el mensaje termina con el caracter #, se completo el mensaje, se procede a la
        // clasificacion del mismo y reenvio por uart
        if (message[strlen(message) - 1] == '#') {
            
            mutex.lock();
            // Copia los caracteres del mensaje al storage
            strcat(data, &message[1]);      // Omite el primer caracter (el cual es 's')
            data[strlen(data) - 1] = '\0';  // Borra el ultimo caracter (el cual es '#')
            memset(message, 0, MSG_LEN);

            // Señaliza a los otros thread que el mensaje fué recibido
            cond.notify_all();
            mutex.unlock();
            break;
        }
    }

    // borra el buffer de recepcion
    memset(recvBuff, 0, BUFF_LEN);
}  

/*

    Evento ante la llegada de caracteres por el puerto UART

*/
void onSigio(void)
{   
    eventQueue.call(onSerialReceived);
}

/*

    Main

*/
int main()
{ 
    // Configuracion del puerto UART (115200-8-N-1)
    serial.set_baud(115200);
    serial.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    ); 

    // Declaracion de los threads
    thread1.start(onSerialSend);
    thread2.start(callback(&eventQueue, &EventQueue::dispatch_forever));
    serial.sigio(callback(onSigio));

    // Led Heartbeat
    while (1) {
        led1 = !led1;       
        ThisThread::sleep_for(500ms);
    }
}