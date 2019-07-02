/*
 * Timerino.c
 *
 * Created: 01/07/2019 10:40:42
 * Author : Luca
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define STATE_WAITING 0
#define STATE_PLAYING 1

uint8_t state;
uint16_t counter;

//Buzzer pin: PB0
//Yellow pin: PB1

//Configured for attiny85/45/25


int main(void)
{
	cli();
	
	//Variable set
	state = STATE_WAITING;
	counter = 0;
	
	//Three state of PB0, is on only when buzzer is on.
	DDRB = _BV(PB1);
	
	//Logic 0 on PB1
	PORTB = 0x00;
    
		
	//Timer 0 configuration.
	
	//Toggle OC0A On Compare (PB0)
	//Set timer on CTC.
	
	TCCR0A = _BV(COM0A0) | _BV(WGM01);
	
	//ClkIO/1024 -> ~1 msec clock period
	//Max counter value ~256 msec.
	
	TCCR0B = _BV(CS02) | _BV(CS00);
	
	//Set a PWM freq of ~500Hz, 50% duty cycle
	OCR0A = 0x00;
	
	//Timer 1 configuration.
	
	//ClkIO/1024 -> ~1 msec clock period
	//Max counter value ~256 msec.
	
	TCCR1 = _BV(CTC1) | _BV(CS13) | _BV(CS11) | _BV(CS10);
	
	//Reset timer
	TCNT1 = 0;
	
	//Set a Period of ~0.25 sec
	OCR1A = 243;
	OCR1C = 243;
	
	//Need a interrupt every 250 msec.
	TIMSK = _BV(OCIE1A);
	
	sei();
	
	while(1);
}

ISR(TIMER1_COMPA_vect)
{
	//Every 250 msec counter++
	counter++;
	
	if(state == STATE_WAITING)
	{
		//A minute is gone.
		if(counter >= 240)
		{
			//Reset counter, change state and start buzzer and led.
			counter = 0;
			
			state = STATE_PLAYING;
			
			DDRB = _BV(PB0) | _BV(PB1);	
			PORTB = _BV(PB1);
		}
	}
	else if(state == STATE_PLAYING)
	{
		//2 secs are gone
		if(counter >= 8)
		{
			//change state and stop buzzer and led.
			//counter = 0;
			
			state = STATE_WAITING;
			
			DDRB = _BV(PB1);
			PORTB = 0x00;
		}
	}	
}