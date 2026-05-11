
#include "usart.h"
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <string.h>
#include <avr/interrupt.h> 

void read_string( uint8_t * str, uint16_t max_size);
void quite();

int main(void)
{   
    initUSART();
    uint8_t recive_buffer[10];
    printString("\r\n Hi, enter command \r\n");
    DDRC |= (1 << PC0);  
    
    while (1){
        read_string(recive_buffer, 10);
        _delay_ms(20);
        if (strcmp(recive_buffer, "on")==0 || strcmp(recive_buffer, "On")==0){
            PORTC |= (1 << PC0);  
            printString("Led turned on.\r\n"); 
        }
        else if(strcmp(recive_buffer, "off")==0 || strcmp(recive_buffer, "Off")==0){
            PORTC &= ~(1 << PC0);
            printString("Led turned off.\r\n");
        }else if(strcmp(recive_buffer, "quite")==0 || strcmp(recive_buffer, "QUITE")==0){
            printString("The app is closing.\r\n");
            quite();
        }
        else{
            printString("Error:Command not found!\r\n");
        }
        _delay_ms(20);
    }
    
    return 0;
}


void read_string( uint8_t * str, uint16_t max_size){
    uint16_t i = 0;
    uint8_t len = max_size -1;
        while (i < len)
        {
            uint8_t c = receiveByte();
            if(c == '\n' || c == '\r'){
                break;
            }
            str[i++] = c;
        }
        
        str[i] = '\0';   
}

void quite()
{
    SP = 0x21FF;
    _delay_ms(2);
    __asm__ __volatile__(
    "cli               \n\t"
    "clr r1            \n\t"
    "jmp 0x3E000       \n\t" 
    );
}