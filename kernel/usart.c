
#include <avr/io.h>
#include "USART.h"
#include <util/setbaud.h>
#include <avr/wdt.h> 

void initUSART(void) {                                /* requires BAUD */
  UBRR0H = UBRRH_VALUE;                        /* defined in setbaud.h */
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif
                                  /* Enable USART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit */
}

void printString(const char myString[])
{
  uint8_t i = 0;
  while (myString[i]) {
    transmitByte(myString[i]);
    i++;
  }
}

void transmitByte(uint8_t data) {
                                     /* Wait for empty transmit buffer */
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data;                                            /* send data */
}

uint8_t receiveByte(void) {
  loop_until_bit_is_set(UCSR0A, RXC0);       /* Wait for incoming data */
  return UDR0;                                /* return register value */
}


                       /* Here are a bunch of useful printing commands */
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

void readString(char myString[], uint8_t maxLength) {
  char response;
  uint8_t i;
  i = 0;
  while (i < (maxLength - 1)) {                   /* prevent over-runs */
    response = receiveByte();
    transmitByte(response);                                    /* echo */
    if (response == '\r') {                     /* enter marks the end */
      break;
    }
    else {
      myString[i] = response;                       /* add in a letter */
      i++;
    }
  }
  myString[i] = 0;                          /* terminal NULL character */
}

void printByte(uint8_t byte) {
              /* Converts a byte to a string of decimal text, sends it */
    uint8_t started = 0;
    if (byte >= 100) {transmitByte('0' + (byte / 100)); started = 1;}                    /* Hundreds */
    if (byte >= 10 || started) { transmitByte('0' + (byte / 10) % 10); started = 1; }                      /* Tens */
    transmitByte('0' + (byte % 10));                             /* Ones */
}

void printWord(uint16_t word) {
    uint8_t started = 0;
    if (word >= 10000) { transmitByte('0' + word / 10000); started = 1; }
    if (word >= 1000 || started) { transmitByte('0' + (word / 1000) % 10); started = 1; }
    if (word >= 100 || started) { transmitByte('0' + (word / 100) % 10); started = 1; }
    if (word >= 10 || started) { transmitByte('0' + (word / 10) % 10); started = 1; }
    transmitByte('0' + (word % 10));
}

void printBinaryByte(uint8_t byte) {
                       /* Prints out a byte as a series of 1's and 0's */
  uint8_t bit;
  for (bit = 7; bit < 255; bit--) {
    if (bit_is_set(byte, bit))
      transmitByte('1');
    else
      transmitByte('0');
  }
}

char nibbleToHexCharacter(uint8_t nibble) {
                                   /* Converts 4 bits into hexadecimal */
  if (nibble < 10) {
    return ('0' + nibble);
  }
  else {
    return ('A' + nibble - 10);
  }
}

void printHexWord(uint16_t word) {
    /* Prints a word (2 bytes) as its hexadecimal equivalent */
    uint8_t nibble;
    
    // Print high byte (first 2 hex digits)
    nibble = (word & 0xF000) >> 12;
    transmitByte(nibbleToHexCharacter(nibble));
    nibble = (word & 0x0F00) >> 8;
    transmitByte(nibbleToHexCharacter(nibble));
    
    // Print low byte (last 2 hex digits)
    nibble = (word & 0x00F0) >> 4;
    transmitByte(nibbleToHexCharacter(nibble));
    nibble = word & 0x000F;
    transmitByte(nibbleToHexCharacter(nibble));
}

void printHexDWORD(uint32_t dword) {
    /* Prints a DWORD (4 bytes) as its hexadecimal equivalent */
    
    // Print highest byte first
    transmitByte(nibbleToHexCharacter((dword >> 28) & 0x0F));
    transmitByte(nibbleToHexCharacter((dword >> 24) & 0x0F));
    
    transmitByte(nibbleToHexCharacter((dword >> 20) & 0x0F));
    transmitByte(nibbleToHexCharacter((dword >> 16) & 0x0F));
    
    transmitByte(nibbleToHexCharacter((dword >> 12) & 0x0F));
    transmitByte(nibbleToHexCharacter((dword >> 8) & 0x0F));
    
    transmitByte(nibbleToHexCharacter((dword >> 4) & 0x0F));
    transmitByte(nibbleToHexCharacter(dword & 0x0F));
}

void printHexByte(uint8_t byte) {
                        /* Prints a byte as its hexadecimal equivalent */
  uint8_t nibble;
  nibble = (byte & 0b11110000) >> 4;
  transmitByte(nibbleToHexCharacter(nibble));
  nibble = byte & 0b00001111;
  transmitByte(nibbleToHexCharacter(nibble));
}

uint8_t getNumber(void) {
    //printString("Waiting for input...\r\n");
    char thisChar = receiveByte();
    //printString("Received char: ");
    transmitByte(thisChar); 
    printString("\r\n");
    char endChar;
    do {
        wdt_reset();
        endChar = receiveByte();
    } while (endChar != '\r' && endChar != '\n');
    if (thisChar < '0' || thisChar > '9') return 0;
    return thisChar - '0'; 
}