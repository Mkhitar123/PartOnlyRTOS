#include <avr/boot.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "diskio.h"
#include "usart.h"

#define LED_ON PORTC |= (1 << PC0)
#define LED_OFF PORTC &= ~(1 << PC0)

#define RAM_FLAG_ADDR       ((volatile uint8_t*)0x1000)
#define RAM_FILENAME_ADDR   ((volatile char*)0x1001)
#define FLESH_ADDRESS 0x00000

#define MAX_FILENAME_LEN 13
#define SPM_PAGESIZE 256

const uint8_t expected_pattern[] = {
    0x4B, 0x45, 0x52, 0x4E, 0x45, 0x4C, 0x5F, 0x4C, 0x49, 0x42, 0x3A, 0x42, 0x55,
    0x49, 0x4C, 0x44, 0x5F, 0x4E, 0x55, 0x4D, 0x42, 0x45, 0x52, 0x3A, 0x30, 0x2E, 0x31
};
#define PATTERN_LEN 27  

FATFS fs;
BYTE Flesh_Buff[SPM_PAGESIZE] = {0xFF};

extern void start_application_at(DWORD byte_addr); // from assembler file 

uint8_t check_kernel_pattern(void) {
    UINT br;
    uint8_t buffer[128];  

    pf_lseek(0); 

    while (pf_read(buffer, sizeof(buffer), &br) == 0 && br > 0) {
        for (uint16_t i = 0; i <= br - PATTERN_LEN; i++) {
            if (memcmp(&buffer[i], expected_pattern, PATTERN_LEN) == 0) {
                return 1; 
            }
        }
    }
    return 0; 
}

void flash_erase(DWORD byte_addr)
{
    DWORD page_addr = byte_addr & ~(SPM_PAGESIZE - 1);
    cli();
    eeprom_busy_wait();
    boot_page_erase(page_addr);
    boot_spm_busy_wait();
    //sei();
}

void flash_write(DWORD byte_addr, const BYTE* buf)
{
    uint16_t i, w;
    DWORD page_addr = byte_addr & ~(SPM_PAGESIZE - 1);
    cli();
    eeprom_busy_wait();
    boot_page_erase(page_addr);
    boot_spm_busy_wait();
    for (i = 0; i < SPM_PAGESIZE; i += 2)
    {
        if (i >= SPM_PAGESIZE) break;
        if (buf[i] == 0xFF && buf[i + 1] == 0xFF)
        {
            w = 0xFFFF;
        } else
        {
            w = ((uint16_t)buf[i] | ((uint16_t)buf[i + 1] << 8));
        }
        boot_page_fill(page_addr + i, w);
    }
    boot_page_write(page_addr);
    boot_spm_busy_wait();
    boot_rww_enable();
    //sei();
}

void n_times_blink_led(uint8_t n)
{
    for (uint8_t i = 0; i < n; i++)
    {
        LED_ON;
        _delay_ms(100);
        LED_OFF;
        _delay_ms(100);
    }
}

int main(void)
{
    // move interrupt vector table into bootloader section
    cli();  
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12); // prescaler 256
    OCR1A = 62499;
    TIMSK1 |= (1 << OCIE1A);
    
    // config red-led  
    DDRC |= (1 << PC0);
    n_times_blink_led(5);

    //watchgod disable 
    wdt_disable();
    initUSART();
    
    printString("Bootloader: Starting\r\n");

    uint8_t res_init = disk_initialize();
    if (res_init != RES_OK)
    {
        printString("Bootloader: Disk init failed\r\n");
        LED_ON;
        while (1);
    }

    uint8_t res_mount = pf_mount(&fs);
    if(res_mount != FR_OK)
    {
        printString("Bootloader: Mount failed\r\n");
        LED_ON;
        while (1);
    } else
    {
        n_times_blink_led(2);
    }

    char filename[MAX_FILENAME_LEN] = "LAUNCHER.BIN";

    // printHexWord(*RAM_FLAG_ADDR);
    // printString("\r\n");
    if (*RAM_FLAG_ADDR == 0xA5)
    {
        strncpy(filename, (char *)RAM_FILENAME_ADDR, MAX_FILENAME_LEN);
        filename[MAX_FILENAME_LEN - 1] = '\0';

        *RAM_FLAG_ADDR = 0;

        printString("Bootloader: Loading ");
        printString(filename);
        printString("\r\n");

        if (strlen(filename) == 0 || strstr(filename, ".BIN") == NULL)
        {
            strcpy(filename, "LAUNCHER.BIN");
            printString("Bootloader: Fallback to LAUNCHER.BIN\r\n");
        }
    } else
    {
        printString("Bootloader: Using default LAUNCHER.BIN\r\n");  
    }
    // printString(filename);
    // printString("\r\n");
    
    uint8_t res_open = pf_open(filename);

    if (res_open == FR_OK)
    {
            if (!check_kernel_pattern())
            {
                printString("ERROR: Missing kernel pattern! return bootloader\r\n");
            }
            else
            {
                n_times_blink_led(3);
                pf_lseek(0);
                DWORD fa = FLESH_ADDRESS; 
                UINT br;
                while (1)
                {
                    memset(Flesh_Buff, 0xFF, SPM_PAGESIZE);
                    _delay_ms(2);
                    uint8_t res = pf_read(Flesh_Buff, SPM_PAGESIZE, &br);
                    if (res != FR_OK || br == 0)
                    {
                        printString("Bootloader: Read finished\r\n");
                        break;
                    }
                    
                    flash_erase(fa);
                    _delay_ms(2);
                    flash_write(fa, Flesh_Buff);
                    _delay_ms(2);
                    
                    fa += SPM_PAGESIZE;
                    if (br < SPM_PAGESIZE) break;
                }
            } 
    }
    else
    {
        printString("Bootloader: Open failed, error: ");
        printWord(res_open);
        printString("\r\n");
        LED_ON;
        while (1);
    }
    

    pf_mount(NULL);
    // printString("\r\n addr --- ");
    // printHexDWORD(*RAM_TARGET_ADDR*2);
    // printString("\r\n--- Debug Info ---\r\n");

    // printString("SP before: 0x");
    // printHexWord(SP);
    // printString("\r\n");

    // printString("RAMEND: 0x");
    // printHexWord(RAMEND);
    // printString("\r\n");
    
    // printString("EIND set to: 0x");
    // printHexByte(EIND);
    // printString("\r\n");

    if (MCUSR & (1 << WDRF))
    {
        printString("WARNING: Watchdog reset flag is set!\r\n");
        MCUSR &= ~(1 << WDRF);
    }
    printString("Bootloader: Starting application\r\n");
    _delay_ms(10);
    SP = RAMEND;
    sei();
    start_application_at(FLESH_ADDRESS);
    while(1);
    return 0;
}