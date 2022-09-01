// General include
#include "mbed.h"

// Pin declaration
DigitalOut led1(LED1);
DigitalOut led2(LED2);

// Serial port inicialization
RawSerial pc(USBTX, USBRX);

// Callback function
void callback_ex() { 
    pc.putc(pc.getc());
    led2 = !led2;
}

//
//    main
//

int main() {

    // Attach callback function to main 
    pc.attach(&callback_ex);

    // Infinite loop
    while (1) {
        // Change state of led in rx port
        led1 = !led1;
        wait(0.5);
    }
}