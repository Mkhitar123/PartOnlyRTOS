#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

/*
    Don`t use timer5 its alredy used by kernel in sheduler instead use timers(1-4).
*/

#define LED_ON PORTC |= (1 << PC0)
#define LED_OFF PORTC &= ~(1 << PC0)
#define LED_TOGGLE PORTC ^= (1 << PC0)

#define MAX_TASKS 8
typedef void (*task_function_type)(void);

void kernel_lib_init(void);
void quite(void);
int task_register(task_function_type function, uint16_t period_ms);
void scheduler(void);
#endif
