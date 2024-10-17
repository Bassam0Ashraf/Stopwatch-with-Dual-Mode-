/* Include header file "Functions_Prototypes.h" */
#include "Functions_Prototypes.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Import variables were defined and declared in file "Functions_Definitions.c" */
extern unsigned char Hrs;
extern unsigned char Mins;
extern unsigned char Secs;
extern unsigned char Flag_Countdown_Mode;
extern unsigned char Flag_Pause;

/*******************************************************************************
 *                     		  Main Program                                     *
 *******************************************************************************/

int main(void)
{
	/* Configure decoder 7447 (PC0 --> PC3) as OUTPUT */
	DDRC  |= 0x0F;													// (0000 1111) Pins for binary to decoder.
	PORTC &= 0xF0; 													// (1111 0000) Initialize PORTC to zero.

	/* Configure 7-segment (PA0 --> PA5) as OUTPUT */
	DDRA  |= 0x3F;													// (0011 1111) Pins for Enable OR Disable each 7-segment.

	/* Configure (PD2, PD3 as INPUT) & (PD0, PD4, PD5 as OUTPUT) */
	DDRD  &= ~((1 << PD2) | (1 << PD3)); 							// PD2, PD3 as INPUT (Reset & Pause button).
	DDRD  |= (1 << PD0) | (1 << PD4) | (1 << PD5); 					// PD0= Buzzer, PD4= Red LED, PD5= yellow LED.

	/* Configure PORTB as INPUT (Increment & Decrement button) */
	DDRB   = 0x00;
	PORTB  = 0xFF; 													// (1111 1111) Enable pull-ups on PORTB.

	/* Enable global interrupts */
	SREG  |= (1 << 7);

	/* Turn ON red LED */
	PORTD |= (1 << PD4);

	/* Turn OFF yellow LED */
	PORTD &= ~(1 << PD5);

	/* Calling all function that we build */
	Timer1_CTC_Init();
	INT0_Falling_Edge_Reset();
	INT1_Rising_Edge_Pause();
	INT2_Falling_Edge_Resume();

	for(;;)
	{
		Time();

		/* Enter countdown mode if PB7 is pressed */
		if ( !(PINB & (1 << PB7)) )
		{
			_delay_ms(30);							// De-bouncing delay.
			if ( !(PINB & (1 << PB7)) )				// Second check due to switch de-bouncing
			{
				Flag_Countdown_Mode ^= 1;				// Count down= 1.
				PORTD ^= (1 << PD5); 				// Set countdown mode LED.
				PORTD ^= (1 << PD4);  				// Turn off normal mode LED.
				while( !(PINB & (1 << PB7)) )		// Stay in countdown mode while button PB7 is pressed.
				{Time();};
			}

		}


		while (Flag_Pause == 1)
		{

			PORTD &= ~(1 << PD0);  					// Turn off BUZZER.
			Time();									// Continuously update the display.

			/* Enter countdown mode if PB7 is pressed */
			if ( !(PINB & (1 << PB7)) )
			{
				_delay_ms(30);						// De-bouncing delay.
				if ( !(PINB & (1 << PB7)) )			// Second check due to switch de-bouncing
				{
					Flag_Countdown_Mode ^= 1;			// Count down= 1.
					PORTD ^= (1 << PD5); 			// Set countdown mode LED.
					PORTD ^= (1 << PD4);  			// Turn off normal mode LED.
					while( !(PINB & (1 << PB7)) )	// Stay in countdown mode while button PB7 is pressed.
					{Time();};
				}

			}


			Time();									// Continuously update the display.

			/* Hours increment */
			if ( !(PINB & INCREMENT_HOURS) )
			{
				_delay_ms(30);						// De-bouncing delay.

				if ( !(PINB & INCREMENT_HOURS) )	// Second check due to switch de-bouncing
				{
					Hrs = (Hrs + 1) % 24;
					while( !(PINB & INCREMENT_HOURS) )
					{Time();};
				}

			}

			/* Hours decrement */
			if ( !(PINB & DECREMENT_HOURS) )
			{
				_delay_ms(30);						// De-bouncing delay.

				if ( !(PINB & DECREMENT_HOURS) )	// Second check due to switch de-bouncing
				{
					Hrs = (Hrs == 0) ? 23 : Hrs - 1;
					while( !(PINB & DECREMENT_HOURS) )
					{Time();}
				}

			}

			/* Minutes increment */
			if ( !(PINB & INCREMENT_MINUTES) )
			{
				_delay_ms(30);						// De-bouncing delay.

				if ( !(PINB & INCREMENT_MINUTES) )	// Second check due to switch de-bouncing
				{
					Mins = (Mins + 1) % 60;
					while( !(PINB & INCREMENT_MINUTES) )
					{Time();}
				}

			}

			/* Minutes decrement */
			if ( !(PINB & DECREMENT_MINUTES) )
			{
				_delay_ms(30);						// De-bouncing delay.

				if ( !(PINB & DECREMENT_MINUTES) )	// Second check due to switch de-bouncing
				{
					Mins = (Mins == 0) ? 59 : Mins - 1;
					while( !(PINB & DECREMENT_MINUTES) )
					{Time();}
				}

			}

			/* Seconds increment */
			if ( !(PINB & INCREMENT_SECONDS) )
			{
				_delay_ms(30);						// De-bouncing delay.

				if ( !(PINB & INCREMENT_SECONDS) )	// Second check due to switch de-bouncing
				{
					Secs = (Secs + 1) % 60;
					while( !(PINB & INCREMENT_SECONDS) )
					{Time();};
				}

			}

			/* Seconds decrement */
			if ( !(PINB & DECREMENT_SECONDS) )
			{
				_delay_ms(30);						// De-bouncing delay.

				if ( !(PINB & DECREMENT_SECONDS) )	// Second check due to switch de-bouncing
				{
					Secs = (Secs == 0) ? 59 : Secs - 1;
					while( !(PINB & DECREMENT_SECONDS) )
					{Time();}
				}

			}

			Time();								  // Continuously update the display
		}

	}

	return 0;
}
