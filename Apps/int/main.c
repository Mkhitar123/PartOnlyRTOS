#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "kernel.h"
#include "usart.h"

// Task 1: Blink LED every 500ms
void blink_led_task(void) {
    PORTC ^= (1 << PC0);
}

// Task 2: Print system heartbeat every 5000ms
void status_report_task(void) {
    printString("\r\n[KERNEL] System is running fine...\r\n");
}

int main(void) {
    task_register(blink_led_task, 500);
    task_register(status_report_task, 5000);

    printString("Kernel App Started\r\n");
    printString("Type 'quite' for exit \r\n");

    char receive_buffer[10];

    while (1) {  

        printString("Enter command: ");
        read_string(receive_buffer, 10);
        
        if(strcasecmp(receive_buffer, "quite") == 0) {
            printString("Closing application\r\n");
            _delay_ms(100); // Give UART time to send the message
            quite();
        } else {
            printString("\r\nYou typed: ");
            printString(receive_buffer);
            printString("\r\n");
        }
    }
}
