#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#include "kernel.h"

// KERNEL_LIB:BUILD_NUMBER:0.1
const uint8_t pattern[] PROGMEM __attribute__((used)) = {
    0x4B, 0x45, 0x52, 0x4E, 0x45, 0x4C, 0x5F, 0x4C, 0x49, 0x42, 0x3A, 0x42, 0x55,
    0x49, 0x4C, 0x44, 0x5F, 0x4E, 0x55, 0x4D, 0x42, 0x45, 0x52, 0x3A, 0x30, 0x2E, 0x31
};

volatile uint32_t sys_time_ms = 0;
volatile uint16_t stack_canary __attribute__((section(".noinit")));
static volatile uint8_t inside_scheduler = 0; 

typedef struct {
    task_function_type run;       
    uint16_t period_ms;  
    uint32_t last_run;   
    uint8_t enabled;    
    volatile uint8_t is_running; 
} task_type;

static task_type tasks[MAX_TASKS];
static uint8_t task_count = 0;

// for not defined interrupts
ISR(BADISR_vect) {}

int task_register(task_function_type function, uint16_t period_ms) {
    if (task_count >= MAX_TASKS) return -1;
    cli(); 
    tasks[task_count].run = function;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].last_run = 0;
    tasks[task_count].enabled = 1;
    tasks[task_count].is_running = 0;
    task_count++;
    sei();
    return task_count - 1;
}

void crash_detection(void) {
    uint16_t sp;
    __asm__ __volatile__ ("in %A0, 0x3D\n\t" // SPL
                          "in %B0, 0x3E"     // SPH
                          : "=r" (sp));
    
    // ATmega2560 RAM starts at 0x0200.
    if (sp < 0x0800 + 400 || stack_canary != 0xDEAD) { 
        wdt_enable(WDTO_15MS);
        while(1);
    }
}

void scheduler(void) {
    if (inside_scheduler) return; 
    inside_scheduler = 1;
    uint32_t now = sys_time_ms;
    for (uint8_t i = 0; i < task_count; i++) {
        if (!tasks[i].enabled) continue;
        if (((uint32_t)(now - tasks[i].last_run) >= tasks[i].period_ms) && !tasks[i].is_running) {
            tasks[i].last_run = now;
            tasks[i].is_running = 1; 
            sei(); 
            tasks[i].run();  
            cli(); 
            tasks[i].is_running = 0; 
        }
    }
    crash_detection(); 
    inside_scheduler = 0;
}

void timer5_init(void) {
    cli(); 
    TCCR5A = 0; 
    TCCR5B = 0;
    TCNT5 = 0;
    OCR5A = 249; // 1ms 16MHz, prescaler 64
    TCCR5B |= (1 << WGM52);      
    TIMSK5 |= (1 << OCIE5A);     
    TCCR5B |= (1 << CS51) | (1 << CS50); 
    sei(); 
}

void kernel_lib_init(void) __attribute__((constructor));
void kernel_lib_init(void) {
    timer5_init();
    initUSART();
    DDRC |= (1 << PC0);
    stack_canary = 0xDEAD;
}

void quite() {
    cli(); 
    TCCR5B = 0; 
    TIMSK5 = 0;
    TIFR5  = 0xFF; 
    UCSR0B = 0; 

    SPL = 0xFF;
    SPH = 0x21;

    _delay_ms(2);
    __asm__ __volatile__(
        "clr r1 \n\t"
        "jmp 0x3E000 \n\t" 
    );
}


ISR(TIMER5_COMPA_vect) {
    sys_time_ms++;
    scheduler(); 
}