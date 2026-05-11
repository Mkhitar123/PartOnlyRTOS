#ifndef SPI_H
#define SPI_H

#define DDR_CS   DDRB
#define PORT_CS  PORTB
#define PIN_CS   PB0

#define DDR_DI   DDRB
#define PORT_DI  PORTB
#define PIN_DI   PB2

#define DDR_CK   DDRB
#define PORT_CK  PORTB
#define PIN_CK   PB1

#define DDR_DO   DDRB
#define PORT_DO  PORTB
#define PIN_DO   PB3

void init_spi (void);		/* Initialize SPI port */


#endif /* SPI_H */