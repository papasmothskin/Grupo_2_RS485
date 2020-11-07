#include <avr/io.h>
#include <util/delay.h>

#define LED PB5
#define ADDR1 PB0
#define ADDR2 PB1
#define ADDR3 PB2
#define ADDR4 PB3
#define WRITE_ENABLE PC0


void setup() {
	DDRB |= (1<<PB5) | (0<<PB0) | (0<<PB1) | (0<<PB2) | (0<<PB3);
	DDRC |= (0<<PC1) | (0<<PC2);
}

void main() {
	setup();
}