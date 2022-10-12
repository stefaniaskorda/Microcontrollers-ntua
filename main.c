#define F_CPU 8000000UL
#include "avr/io.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/*
~ T? ???????????? ~
	PC0	PC1	PC2	PC3 <---????????
	1	2	3	?	| PC4
	4	5	6	?	| PC5
	7	8	9	C	| PC6
	*	0	#	D	| PC7
					  ^- ?????????????
	PINC= xxxx?321, xxxxB654, xxxxC987, xxxxD#0*
		     PC4      PC5       PC6       PC7	= 1 ??? ? ???????? PORTC=0 	
*/

char scan_row_sim(char load){
	PORTC = load;
	_delay_us(500);
	char temp = PINC;
	return (temp & 0x0f);
}
uint16_t scan_keypad_sim(){
	char temp, a, b;
	temp = scan_row_sim(0x10);
	PORTC = 0;
	temp <<= 4;
	a = temp;
	temp = scan_row_sim(0x20);
	PORTC = 0;
	a += temp;
	temp = scan_row_sim(0x40);
	PORTC = 0;
	temp <<= 4;
	b = temp;
	temp = scan_row_sim(0x80);
	PORTC = 0;
	b += temp;
	uint16_t asd = ((a << 8) & 0xff00) | b;
	return asd;
}

uint16_t palio;
uint16_t scan_keypad_rising_edge_sim(){
	// static uint16_t palio; ???
	//debouncing
	uint16_t temp1 = scan_keypad_sim();
	_delay_ms(15);
	uint16_t temp2 = scan_keypad_sim();
	
	//check for new keys
	uint16_t ap_1 = temp1&temp2;
	uint16_t res = ap_1&(palio^0xffff);
	palio = ap_1;
	return res;
}

char keypad_to_ascii_sim(uint16_t keyb){
	char ascii[16] = {'A','3','2','1','B','6','5','4','C','9','8','7','D','#','0','*'};
	for(int i=0; i<=15; ++i){
		if((keyb&(0x1)) == 0x1){
			return ascii[15-i];
		}
		keyb >>=1;
	}
	return 0;
}

// Display:

// ?????? enable
void pulse_enable() {
	PORTD |= (1 << PD3); //asm("sbi PORTD, PD3");
	PORTD &= ~(1 << PD3); //asm("cbi PORTD, PD3");
}

// ?????? 1 byte: 4 bit ?? ????, MSB->LSB.
void write_2_nibbles_sim(char mybyte) { // data or command
	_delay_ms(6);
	char pd_low = PIND & 0x0F;
	PORTD = (mybyte & 0xF0) + pd_low; // PD4-PD7 = cmd_high
	pulse_enable();
	_delay_ms(6);
	PORTD = (mybyte << 4) + pd_low; // PD4-PD7 = cmd_low
	pulse_enable();
}

//?????? 1 byte ?????????
void lcd_data_sim(char data) {
	PORTD |= (1 << PD2); // ??????? ??? ?????????? ????????? ("sbi PORTD, PD2")
	write_2_nibbles_sim(data);
	_delay_us(43);
}

//?????? 1 byte ???????
void lcd_command_sim(char cmd) {
	PORTD &= ~(1 << PD2); // ??????? ??? ?????????? ????????? ("cbi PORTD, PD2")
	write_2_nibbles_sim(cmd);
	_delay_us(39);
}

//???????????? ??? ??????:
//	DL=0 4-bit mode
//	N=1 2 lines
//	F=0 5�8 dots
//	D=1 display on
//	C=0 cursor off
//	B=0 blinking off
//	I/D=1 DDRAM address auto increment
//	SH=0 shift of entire display off
void lcd_init_sim() {
	_delay_ms(40);
	for(int i=1; i<=2; ++i) {
		PORTD = 0x30; // ???????? ?? 8-bit mode
		pulse_enable();
		_delay_us(39);
		_delay_us(1000);
	}
	PORTD = 0x20; // ???????? ?? 4-bit mode
	pulse_enable();
	_delay_us(39);
	_delay_us(1000);
	lcd_command_sim(0x28); // 5x8 ????????, 2 ???????
	lcd_command_sim(0x0c); // screen ON, cursor OFF
	lcd_command_sim(0x01); // clear screen
	_delay_us(1530);
	lcd_command_sim(0x06); // autoinc addr, ???????? OFF
}

void screen_write(char* msg) {
	for(int i=0; msg[i]!='#'; ++i) { // terminate with #
		lcd_data_sim(msg[i]);
	}
}

void ADC_init() {
	//DDRA = 0;				// PINA0 -> ADC input pin = input
	ADMUX = (1 << REFS0);	// Vref: Vcc (5V for easyAVR6), MUX4:0 = 00000 => read A0.
	ADCSRA = (1 << ADEN);	// ADC ON (ADIE OFF, not ADC sei)
}

char FIRST_LINE[] = "Vo1\n#";
char num[5] = "x.xx#";
const int MAX_CVOLTS = 500;

//OC0 is connected to pin PB3
//OC1A is connected to pin PD5
//OC2 is connected to pin PD7
void PWM_init()
{
	// Clear on Compare match (CTC) Mode
	//set TMR0 in fast PWM mode with non-inverted output, prescale=8 (1MHz)
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
	DDRB |= (1<<PB3); // set PB3 pin as output of OC0
}

// uncomment below for exactly 4MHz
/*
ISR (TIMER0_OVF_vect)    // Timer1 ISR
{
	TCNT0 = 6;   // for 4 MHz PWM frequency
}
*/


int main ()
{
	uint16_t keyb = 0;	// keyboard input
	char dig = 0;		// digit from keyboard
	DDRD = 0xFF;		// ?????
	DDRC = 0xF0;		// PORTC -> ????????????: OOOOIIII (Out=1, In=0)
	
	ADC_init();
	PWM_init();
	lcd_init_sim();

	sei();
	char oldnum[4] = "0.00";
	OCR0 = 0;
	int a;
	
	while (1)
	{
		if ((keyb = scan_keypad_rising_edge_sim()) != 0){
			dig = keypad_to_ascii_sim(keyb);
			if (dig == '1' && OCR0 < 255){
				//increase_duty_cycle
				OCR0++;
			}
			else if (dig == '2' && OCR0 > 0){
			// uncomment below for exactly 4MHz
			// if (OCR0 > 6)
				// decrease_duty_cycle
				OCR0--;
			}
		}

		ADCSRA |= (1 << ADSC);		// start conversion
		while(ADCSRA & (1<<ADSC));	// wait for conversion to complete
		
		a = ADC * (MAX_CVOLTS / 1024.0); // 0-1024 -> 0-MAX_CVOLTS (??????? ????? ADCL ??? ???? ADCH.)

		num[0] = (a/100) + '0';
		//num[1] = '.';
		num[2] = ((a%100)/10) + '0';
		num[3] = (a%10) + '0';
		
		if (oldnum[0] != num[0] || oldnum[2] != num[2] || oldnum[3] != num[3]){
			lcd_command_sim(0x01); // clear screen
			screen_write(FIRST_LINE);
			screen_write(num);
			oldnum[0] = num[0];
			oldnum[2] = num[2];
			oldnum[3] = num[3];
			_delay_ms(100);
		}
	}
}
