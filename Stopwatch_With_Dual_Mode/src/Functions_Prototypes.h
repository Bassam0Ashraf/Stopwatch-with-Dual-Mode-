#ifndef SRC_FUNCTIONS_PROTOTYPES_H_
#define SRC_FUNCTIONS_PROTOTYPES_H_

/* Include library that we will use */
#include <avr/io.h>						// To use register on microcontroller.
#include <avr/interrupt.h>				// To use interrupt.
#include <util/delay.h>					// To use delay functions.

#define INCREMENT_HOURS   (1 << PB1)	// Button increment hour.
#define DECREMENT_HOURS   (1 << PB0)	// Button decrement hour.
#define INCREMENT_MINUTES (1 << PB4)	// Button increment minutes.
#define DECREMENT_MINUTES (1 << PB3)	// Button decrement minutes.
#define INCREMENT_SECONDS (1 << PB6)	// Button increment seconds.
#define DECREMENT_SECONDS (1 << PB5)	// Button decrement seconds.

/*******************************************************************************
 *                           Functions Prototypes                              *
 *******************************************************************************/

/* Function to set value one digit from 6 digits of timer */
void Digit_And_Position(unsigned char Digit, unsigned char Position);

/* Function to display all digits at same time */
void Time(void);

/* Function Enable Timer1 (Compare mode is used) to calculate 1 Second */
void Timer1_CTC_Init(void);

/* Function Enable Interrupt 0 (Falling edge mode is used) */
void INT0_Falling_Edge_Reset(void);

/* Function Enable Interrupt 1 (Rising edge mode is used) */
void INT1_Rising_Edge_Pause(void);

/* Function Enable Interrupt 2 (Falling edge mode is used) */
void INT2_Falling_Edge_Resume(void);


/* Interrupt Service Routine for Timer1 */
ISR(TIMER1_COMPA_vect);

/* Interrupt Service Routine for Interrupt 0 */
ISR(INT0_vect);

/* Interrupt Service Routine for Interrupt 1 */
ISR(INT1_vect);

/* Interrupt Service Routine for Interrupt 2 */
ISR(INT2_vect);


#endif /* SRC_FUNCTIONS_PROTOTYPES_H_ */
