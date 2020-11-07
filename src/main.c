#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>


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

#define ST_IDDLE 0
#define ST_REC_ADDR 1
#define ST_REC_DATA 2
#define ST_IGNORE 3

#define ST_SEND_1 1
#define ST_SEND_2 2
#define ST_SEND_DATA 3


#define LOW 0
#define HIGH 1

uint8_t address = 0;			// Endereço como variável global (0 se master)
uint8_t state = ST_IDDLE;		// Estado da máquina de estados
uint8_t prev_led_state_1 = LOW;	// Estado anterior do led 1
uint8_t prev_led_state_2 = LOW;	// Estado anterior do led 2

{
void init_usart(){
	// definir baudrate
	UBRR0H = (uint8_t)(UBBR_VAL>>8);
	UBRR0L = (uint8_t)(UBBR_VAL);

	UCSR0A = (1<<MPCM0); // Modo multiprocessor

	UCSR0C = (3<<UCSZ00) | (3<<UPM00) | (0<<USBS0); // 8-bits, paridade impar, 1 stop bit

	// ler primeiro o endereço para ver se é cliente ou servidor
	if (address > 0){
		UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<RXB80); // Aceita receção, e leitura 9º bit
	}
	else{
		UCSR0B = (1<<TXCIE0) | (1<<TXEN0) | (1<<RXB80); // Aceita transmissão, e leitura 9º bit
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
	address = PINB & ((1<<ADDR1) | (1<<ADDR2) | (1<<ADDR3) | (1<<ADDR4));
}

void sendSlave1(uint8_t button){

	state = ST_SEND_1;
}

void sendSlave2(uint8_t button){
	// Envia trama de endereço
	state = ST_SEND_2;
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
	sei();
}
}

ISR (USART_RX_vect){

}

ISR (USART_TX_vect){
	if (ST_SEND_1){
		
		state = ST_SEND_DATA;
	}
	else if (ST_SEND_2){

		state = ST_SEND_DATA;
	}
	else if (ST_SEND_DATA){

	}
}

void main() {
	setup();
	
	if (address == 0){
		uint8_t button1;
		uint8_t button2;
		while(1){
			button1 = readButton1();
			button2 = readButton2();
			
			if (state == ST_SEND_DATA || state == ST_SEND_1 || state == ST_SEND_2){

			}
			else if (button1 != prev_led_state_1){
				sendSlave1(button1);
				prev_led_state_1 = button1;
			}
			else if (button2 != prev_led_state_2){
				sendSlave2(button2);
				prev_led_state_2 = button2;
			}
		}
	}
	else {
		state = ST_IDDLE;
		while(1){
			switch (state){
				case ST_IDDLE:
					break;
				case ST
			}
		}
	}
	
}