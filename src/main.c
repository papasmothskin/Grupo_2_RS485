#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#define BAUD 9600
#define UBBR_VAL ((F_CPU/(BAUD<<3))-1)


#define LED PB5
#define WRITE_ENABLE PC0
#define BUTTON1 PC1
#define BUTTON2 PC2

#define ADDR1 PB0
#define ADDR2 PB1
#define ADDR3 PB2
#define ADDR4 PB3


int ADDR; // Endereço como variável global (0 se master)

void init_usart(){
	// definir baudrate
	UBRR0H = (uint8_t)(UBBR_VAL>>8);
	UBRR0L = (uint8_t)(UBBR_VAL);

	UCSR0A = (1<<MPCM0); // Modo multiprocessor

	UCSR0C = (3<<UCSZ00) | (3<<UPM00) | (0<<USBS0); // 8-bits, paridade impar, 1 stop bit

	// ler primeiro o endereço para ver se é cliente ou servidor
	if (ADDR > 0){
		UCSR0B = (1<<RXEN0) | (1<<RXB80); // Aceita receção, e leitura 9º bit
	}
	else{
		UCSR0B = (1<<TXEN0) | (1<<RXB80); // Aceita transmissão, e leitura 9º bit
	}
}

void writeLED_H(){
	PORTB |= (1<<LED);
}

void writeLED_L(){
	PORTB &= ~(1<<LED); 
}

uint8_t readButton1(){
	return PINC & (1<<BUTTON1);
}

uint8_t readButton2(){
	return PINC & (1<<BUTTON2);
}

void readAddress(){
	ADDR = PINB & ((1<<ADDR1) | (1<<ADDR2) | (1<<ADDR3) | (1<<ADDR4));
}

void setup() {
	// Outputs
	DDRB |= (1<<LED);
	DDRC |= (1<<WRITE_ENABLE);

	// Inputs
	DDRB &= ~((1<<ADDR1 | (1<<ADDR2) | (1<<ADDR3) | (1<<ADDR4)));
	DDRC &= ~((1<<BUTTON1) & (0<<BUTTON2));

	readAddress();

	init_usart();
}

void main() {
	setup();

	
}