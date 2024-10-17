/* Include header file "Functions_Prototypes.h" */
#include "Functions_Prototypes.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Define and declared variables for Hours,Minutes,Seconds,Flag countdown, Flag Pause */
unsigned char Hrs = 0;
unsigned char Mins = 0;
unsigned char Secs = 0;
unsigned char Flag_Countdown_Mode = 0;
unsigned char Flag_Pause = 0;

/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/* Function to set value one digit from 6 digits of timer */
void Digit_And_Position(unsigned char Digit, unsigned char Position)
{
	/* Turn off all digit positions (PORTA controls which digit is active) */
	PORTA = (PORTA & 0xC0) | (1<< Position);

	/* Set the 7-segment data (The lower 4 bits at PORTC PC0 --> PC3) */
	PORTC = (PORTC & 0xF0) | (Digit & 0x0F);
}


/* Function to display all digit at same time */
void Time(void)
{
	/* Display seconds */
	Digit_And_Position(Secs % 10, 5);  		// First digit of seconds.
	_delay_ms(1);
	Digit_And_Position(Secs / 10, 4);  		// Second digit of seconds.
	_delay_ms(1);

	/* Display minutes */
	Digit_And_Position(Mins % 10, 3);  		// First digit of minutes.
	_delay_ms(1);
	Digit_And_Position(Mins / 10, 2);  		// Second digit of minutes.
	_delay_ms(1);

	/* Display hours */
	Digit_And_Position(Hrs % 10, 1);  		// First digit of hours.
	_delay_ms(1);
	Digit_And_Position(Hrs / 10, 0);  		// Second digit of hours.
	_delay_ms(1);
}


/* Function Enable Timer1 (Compare mode is used) to calculate 1 Second */
void Timer1_CTC_Init(void)
{
	/* Set FOC1A & FOC1B for non PWM.
	 * Set WGM12 to select mode number 4 CTC.
	 * Set CS10 & CS12 to select Prescaler 1024 (Fcpu/1024). */
	TCCR1A = (1<< FOC1A) | (1<< FOC1B);
	TCCR1B = (1<< WGM12) | (1<< CS10) | (1<< CS12);

	/* Fcpu = 16 MHz & N(Prescaler) = 1024
	 * So the Ttime = 63.81 us
	 * Tcompare= Ttime X compare value
	 * Tcompare= (63.81 us) X (15672) = 1.00003032 sec*/

	/* Set initial value for Timer1 and set compare value. */
	TCNT1 = 0;										// Initial value.
	OCR1A = 15672;									// Compare value.

	/* Enable Timer1 interrupt */
	TIMSK |= (1<< OCIE1A);

}


/* Interrupt Service Routine for Timer1 */
ISR(TIMER1_COMPA_vect)
{
	/* This ISR handles the counting of timer and if timer will count up or down */

	/* Count Down mode */
	if (Flag_Countdown_Mode)
	{
		if(Secs == 0)
		{
			if (Mins == 0)
			{
				if(Hrs == 0)
				{
					PORTD |= (1<< PD0);				// Turn ON Buzzer.
					return;							// Count Down finished.
				}
				Hrs--;
				Mins= 59;
				Secs= 59;
			}
			else
			{
				Mins--;
				Secs= 59;
			}
		}
		else
		{
			Secs--;
		}
	}

	/* Count Up mode */
	else
	{
		PORTD |= (1<< PD4);						// Turn ON Red led.
		PORTD &=~ (1<< PD0);					// Turn OFF Buzzer.
		Secs++;
		if(Secs >= 60)
		{
			Secs= 0;
			Mins++;
			if (Mins >= 60)
			{
				Mins= 0;
				Hrs++;
				if(Hrs >= 24)
				{
					Hrs= 0;						// Reset after 24 hours.
				}

			}
		}
	}
}


/* Function Enable Interrupt 0 (Falling edge mode is used) */
void INT0_Falling_Edge_Reset(void)
{
	/* Make the connected pin to push button as input pin */
	DDRD  &=~ (1<< PD2);

	/* Activate the internal pull up */
	PORTD |= (1<< PD2);

	/* Enable Falling edge */
	MCUCR |= (1<< ISC01);
	MCUCR &=~ (1<< ISC00);

	/* Enable INT0 & I-bit(Global Interrupt) */
	GICR  |= (1<< INT0);
	SREG  |= (1<< 7);
}


/* Interrupt Service Routine for Interrupt 0 */
ISR(INT0_vect)
{
	/* This ISR handles if pressed the (push button PD2) Stop Watch time should be reset */

	TCNT1 = 0;
	Secs = 0;
	Mins = 0;
	Hrs = 0;
}


/* Function Enable Interrupt 1 (Rising edge mode is used) */
void INT1_Rising_Edge_Pause(void)
{
	/*configure Pause Button (PD3 INT1) */
	DDRD &=~ (1<< PD3);

	/* Enable Rising edge */
	MCUCR |= (1<< ISC10) | (1<< ISC11);

	/* Enable INT0 & I-bit(Global Interrupt) */
	GICR  |= (1<< INT1);
	SREG  |= (1<< 7);

	TCNT1 = 0;					// Pauses timer.
	PORTD &=~ (1<< PD0);		// Turn OFF Buzzer.
}


/* Interrupt Service Routine for Interrupt 1 */
ISR(INT1_vect)
{
	/* This ISR handles if pressed the (push button PD3) the Stop Watch time should be paused */

	TCCR1B &= 0xF8;													// Pauses timer.
	Flag_Pause= 1;
}


/* Function Enable Interrupt 2 (Falling edge mode is used) */
void INT2_Falling_Edge_Resume(void)
{
	/* Make the connected pin to push button (PB2) as INPUT pin */
	DDRB  &=~ (1<< PB2);

	/* Activate the internal pull up */
	PORTB |= (1<< PB2);

	/* Enable Falling edge */
	MCUCSR &= ~(1 << ISC2);

	/* Enable INT2 & I-bit(Global Interrupt) */
	GICR |= (1 << INT2);
	SREG  |= (1<< 7);
}


/* Interrupt Service Routine for Interrupt 2 */
ISR(INT2_vect)
{
	/* This ISR handles if pressed the (push button PB2) Stop Watch time should be resumed */

	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); 				// Resumed Timer1.
	PORTD &=~ (1<< PD0);											// Turn OFF Buzzer.
	Flag_Pause= 0;
}

