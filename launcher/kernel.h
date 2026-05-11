#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
/*
    -- ! dont use timer5  because it uses by kernel
    -- for crash protection. Instead use timer(0-4)  
*/
void kernel_lib_init(void);
void assert_loop(void);
#endif
