#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>


#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#define BAUD 9600
#define UBBR_VAL ((F_CPU/(BAUD<<4))-1) // Há problemas com o valor, não é o correto, tarde demais para experimentar algo mais elegante


#define LED PB5
#define WRITE_ENABLE PC0
#define BUTTON1 PC1
#define BUTTON2 PC2

#define ADDR1 PB0
#define ADDR2 PB1
#define ADDR3 PB2
#define ADDR4 PB3

// #define PARITY_BIT

#define LOW 0
#define HIGH 1

uint8_t address = 0;			// Endereço como variável global (0 se master)
uint8_t prev_led_state_1 = LOW;	// Estado anterior do led 1
uint8_t prev_led_state_2 = LOW;	// Estado anterior do led 2

void writeLED_H(){
	PORTB &= ~(1<<LED);
}

void writeLED_L(){
	PORTB |= (1<<LED);
}

void writeLED(uint8_t is_high){
	if (is_high) writeLED_H();
	else writeLED_L();
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

void enable_USART_read(){
	PORTC &= ~(1 << WRITE_ENABLE);
}

void enable_USART_write(){
	PORTC |= (1 << WRITE_ENABLE);
}

// https://exploreembedded.com/wiki/UART_Programming_with_Atmega128
void init_usart(){
	// definir baudrate
	long int MYUBRR=((F_CPU)/(BAUD*16UL)-1);
	UBRR0H = (uint8_t)(MYUBRR>>8);
	UBRR0L = (uint8_t)(MYUBRR);

	UCSR0C = (3<<UCSZ00); // 8-bits, 1 stop bit
	#ifdef PARITY_BIT
		UCSR0C |= (3<<UPM00); // paridade impar
	#endif
	UCSR0B = (1<<UCSZ02); // Activate 9th bit

	// ler o endereço para ver se é cliente ou servidor
	if (address > 0){
		UCSR0B |= (1<<RXCIE0) | (1<<RXEN0); // Aceita receção e interrupção
		UCSR0A |= (1<<MPCM0); // Modo multiprocessor
		enable_USART_read();
	}
	else{
		UCSR0B |= (1<<TXEN0); // Aceita transmissão
		enable_USART_write();
	}
}

void setup() {
	// Outputs
	DDRB |= (1<<LED);
	DDRC |= (1<<WRITE_ENABLE);

	// Inputs
	DDRB &= ~((1<<ADDR1 | (1<<ADDR2) | (1<<ADDR3) | (1<<ADDR4)));
	DDRC &= ~((1<<BUTTON1) & (0<<BUTTON2));

	readAddress();

	cli();
	init_usart();
	
	sei();
}

// ############## MASTER #######################

void transmitMessage(uint8_t is_address, uint8_t value){
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));

	/* Copy 9th bit to TXB8 */
	UCSR0B &= ~(1<<TXB80);
	if(is_address){
		UCSR0B |= (1<<TXB80);
	}
	/* Put data into buffer, sends the data */
	UDR0 = value;
}

uint8_t prev_sent_addr = 0;

void sendSlave(uint8_t sending_addr, uint8_t button){
	if (prev_sent_addr != sending_addr){
		transmitMessage(1, sending_addr);
		prev_sent_addr = sending_addr;
	}
	transmitMessage(0,button);
}

// ############## SLAVE #######################

ISR (USART0_RX_vect){

	unsigned char status, is_address, result_low, result_high;
	/* Wait for data to be received */
	// while (!(UCSR0A & (1<<RXC0)));


	/* Get status and 9th bit, then data */
	/* from buffer */
	status = UCSR0A;
	result_high = UCSR0B;
	result_low = UDR0;
	is_address = (result_high>>1) & 0x01;

	/* If error, ... */
	if (status & ((1<<FE0)|(1<<DOR0)|(1<<UPE0))){
		return;
	}

	if (is_address){
		if (result_low == address){
			UCSR0A &= ~(1<<MPCM0); // Desativa modo multiprocessor, este slave foi selecionado
		}
		else {
			UCSR0A |= (1<<MPCM0); // Reativa modo multiprocessor
		}
		
	}
	else {
		writeLED(result_low);
	}
}

int main() {
	setup();
	
	if (address == 0){
		uint8_t button1;
		uint8_t button2;
		while(1){
			button1 = readButton1();
			button2 = readButton2();
			
			if (button1 != prev_led_state_1){
				sendSlave(1,button1);
				prev_led_state_1 = button1;
			}
			else if (button2 != prev_led_state_2){
				sendSlave(2,button2);
				prev_led_state_2 = button2;
			}
		}
	}
	else {
		while(1){
	
		}
	}
	return 0;
}
