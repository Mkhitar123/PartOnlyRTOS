#include "usart.h"
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h> 
#include <avr/wdt.h>       
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "pff.h"
#include "pffconf.h"
#include "kernel.h"  

#define RAM_FLAG_ADDR ((uint8_t *)0x1000)
#define RAM_FILENAME_ADDR ((char *)0x1001)
#define MAX_FILENAME_LEN 13 
#define BOOTLOADER_ADDRESS 0x3E000


typedef struct {
    char **names;
    WORD count;
}bin_files;


FATFS fs;
DIR dir;
FILINFO fno;
bin_files *bfs;

void free_bin_files(bin_files *bfs){
    for(size_t i = 0; i<bfs->count; i++){
        free(bfs->names[i]);
    }
    free(bfs->names);
    bfs->names = NULL;
    bfs->count = 0;
}

int is_binary_ext(const char *fname) {
    const char *ext = strrchr(fname, '.');
    if (!ext) return 0;
    ext++;  // skip the dot

    char lowext[4];
    int i = 0;
    while (*ext && i < (int)sizeof(lowext) - 1)
        lowext[i++] = (char)tolower((unsigned char)*ext++);
    lowext[i] = 0;

    return (strcmp(lowext, "bin") == 0);
}

void list_files(void) {
    FRESULT res;

    res = pf_mount(&fs);
    if (res != FR_OK) {
        printString("Mount failed\r\n");
        return;
    }

    res = pf_opendir(&dir, "");
    if (res != FR_OK) {
        printString("Open dir failed\r\n");
        return;
    }

    printString("Binary files found:\r\n");
    int file_count = 0;
    for (;;) {
        res = pf_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0)
            break; // no more files or error

        // Skip directories
        if (fno.fattrib & AM_DIR) continue;
        // Skip hidden
        if (fno.fattrib & AM_HID) continue;
        // Skip system
        if (fno.fattrib & AM_SYS) continue;

        // Check .bin extension
        if (is_binary_ext(fno.fname)) {
            
            char **tmp = realloc(bfs->names, (bfs->count + 1) * sizeof(char*));
            if (!tmp) printString("\r\nError : Memory allocation. \r\n");

            bfs->names = tmp;
            
            bfs->names[bfs->count] = malloc(strlen(fno.fname) + 1);
            if (!bfs->names[bfs->count]) printString("\r\nError : Memory allocation. \r\n");

            strcpy(bfs->names[bfs->count], fno.fname);
            bfs->count++;
        
            file_count +=1;
            printWord(bfs->count); 
            printString(") ");
            printString(fno.fname);
            printString("  size: ");
            printWord(fno.fsize);
            printString("  bytes");
            printString("\r\n");            
        }
    }
}

void load_and_run_app(const char *filename)
{
    if (filename == NULL || strlen(filename) == 0) {
        printString("Error: Invalid filename\r\n");
        return;
    }

    cli();                                   
    wdt_disable();                          

    *RAM_FLAG_ADDR = 0xA5;

    memset(RAM_FILENAME_ADDR, 0, MAX_FILENAME_LEN);
    strncpy(RAM_FILENAME_ADDR, filename, MAX_FILENAME_LEN - 1);
    RAM_FILENAME_ADDR[MAX_FILENAME_LEN - 1] = '\0';

    printString("Loading application to Flash...\r\n");

    // Jump  bootloader boot section
    __asm__ volatile("jmp 0x3E000" :::);

    while(1);
}

void clear_all_bootloader_ram(void)
{
    *RAM_FLAG_ADDR = 0;
    
    volatile char *filename_ptr = RAM_FILENAME_ADDR;
    for(uint8_t i = 0; i < MAX_FILENAME_LEN; i++) {
        filename_ptr[i] = 0;
    }
    
    _delay_ms(1);
}

int main(void)
{   
    
    clear_all_bootloader_ram();
    initUSART();
    DDRC |= (1 << PC0); // LED pin 

    printString("\r\n=== Part only real time OS Launcher v1.0 ===\r\n");
    cli();
    while (1)
    {
        assert_loop();
        bfs = malloc(sizeof(bin_files));
        if (!bfs)
        {
            printString("Memory error!\r\n");
            _delay_ms(200);
            continue;
        }
        bfs->names = NULL;
        bfs->count = 0;

        *RAM_FLAG_ADDR = 0;
        memset((char *)RAM_FILENAME_ADDR, 0, MAX_FILENAME_LEN);
        
        list_files();
        if (bfs->count == 0)
        {
            printString("\r\nNo applications found on SD card.\r\n");
            _delay_ms(200);
        } else
          {
            printString("\r\nEnter application number to run.\r\n");
            uint8_t selected = getNumber();

            if (selected >= 1 && selected <= bfs->count)
            {
                printString("Loading: ");
                printString(bfs->names[selected - 1]);
                printString("\r\n");
                load_and_run_app(bfs->names[selected - 1]);
                
            } else
            {
                printString("Invalid selection. \r\n");
            }
        }

        free_bin_files(bfs);
        free(bfs);
        bfs = NULL;
        _delay_ms(100);
        

    }

    return 0;
}