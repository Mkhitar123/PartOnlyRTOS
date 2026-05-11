
#include "spi.h"
#include <avr/io.h>

void init_spi(void)
{
    DDR_CS  |= (1 << PIN_CS);
    DDR_DI  |= (1 << PIN_DI);
    DDR_CK  |= (1 << PIN_CK);

    DDR_DO  &= ~(1 << PIN_DO);   // clear bit = input
    PORT_DO |= (1 << PIN_DO);    // set bit = pull-up
}
