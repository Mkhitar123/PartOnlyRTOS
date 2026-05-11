#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <avr/io.h>       
#include <avr/interrupt.h>

// Kernel Configuration
#define MAX_TASKS 8

// Helper Macros
#define LED_PORT PORTC
#define LED_PIN  PC0
#define LED_ON     (LED_PORT |= (1 << LED_PIN))
#define LED_OFF    (LED_PORT &= ~(1 << LED_PIN))
#define LED_TOGGLE (LED_PORT ^= (1 << LED_PIN))

typedef void (*task_function_type)(void);

// External global variables 
extern volatile uint32_t sys_time_ms;

// API
void kernel_lib_init(void);
void quite(void);
int task_register(task_function_type function, uint16_t period_ms);
void scheduler(void);

#endif