# Dual-Mode Stopwatch System

**ATmega32-Based Digital Stopwatch with Increment and Countdown Functionality**

## Table of Contents
- [Project Overview](#project-overview)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Operational Modes](#operational-modes)
- [User Interface](#user-interface)
- [Timer Configuration](#timer-configuration)
- [Display System](#display-system)
- [Interrupt Handling](#interrupt-handling)
- [Control Logic](#control-logic)
- [Hardware Connections](#hardware-connections)
- [Software Implementation](#software-implementation)
- [Development Environment](#development-environment)
- [Operation Procedure](#operation-procedure)
- [License](#license)

## Project Overview

This project implements a sophisticated dual-mode digital stopwatch using the ATmega32 microcontroller operating at 16MHz. The system features two distinct operational modes: increment counting (stopwatch) and countdown timing, with comprehensive user controls for time adjustment, mode switching, and operation control.

The implementation demonstrates advanced embedded systems concepts including timer configuration in CTC mode, external interrupt handling, seven-segment multiplexing techniques, and real-time user interface management.

### System Simulation
The complete system has been designed and tested using Proteus Design Suite, demonstrating real-world hardware behavior and user interaction.

![Stopwatch](https://github.com/user-attachments/assets/04b0269b-36e9-4969-a69d-dd6401d4c837)

## System Architecture

### Functional Block Diagram
```
┌─────────────────────────────────────────┐
│         User Interface Layer            │
│         ├── 10 Push Buttons             │
│         ├── Visual Indicators (LEDs)    │
│         └── Audio Alerts (Buzzer)       │
├─────────────────────────────────────────┤
│         Display Layer                   │
│         ├── Six 7-Segment Displays      │
│         ├── BCD to 7-Segment Decoder    │
│         └── Multiplexing Control        │
├─────────────────────────────────────────┤
│         Control Layer                   │
│         ├── Mode Management             │
│         ├── Time Calculations           │
│         └── State Machine Logic         │
├─────────────────────────────────────────┤
│         Hardware Abstraction Layer      │
│         ├── Timer1 CTC Configuration    │
│         ├── External Interrupts (0,1,2) │
│         └── GPIO Management             │
├─────────────────────────────────────────┤
│         Hardware (ATmega32)             │
└─────────────────────────────────────────┘
```

## Hardware Components

### Core Processing Unit
- **Microcontroller**: ATmega32 @ 16MHz
- **Architecture**: 8-bit AVR RISC processor with 32KB Flash, 2KB SRAM
- **Timer Module**: Timer1 configured in CTC mode for precise 1-second intervals

### Display System
- **Primary Display**: Six multiplexed seven-segment displays (common anode)
- **Format**: HH:MM:SS (Hours:Minutes:Seconds)
- **Decoder**: 7447 BCD to seven-segment decoder
- **Control**: PORTC (PC0-PC3) for BCD data, PORTA (PA0-PA5) for digit selection
- **Multiplexing**: NPN BJT transistors for individual digit enable/disable control

### User Interface Components
- **Control Buttons**: 10 push buttons for comprehensive system control
- **Visual Indicators**: 
  - Red LED (PD4): Increment mode indicator
  - Yellow LED (PD5): Countdown mode indicator
- **Audio Alert**: Buzzer (PD0) for countdown completion notification

### Input Controls
- **System Control**:
  - Reset: PD2 (INT0) with internal pull-up
  - Pause: PD3 (INT1) with external pull-down  
  - Resume: PB2 (INT2) with internal pull-up
  - Mode Toggle: PB7 with internal pull-up
- **Time Adjustment**:
  - Hours: PB1 (increment), PB0 (decrement)
  - Minutes: PB4 (increment), PB3 (decrement) 
  - Seconds: PB6 (increment), PB5 (decrement)

## Operational Modes

### Increment Mode (Default)
**Functionality**: Counts upward from 00:00:00
**Characteristics**:
- Automatic start on power-up
- Continuous counting until paused or reset
- 24-hour format with automatic rollover
- Red LED indicator (PD4) active
- Timer increments: Seconds → Minutes → Hours

### Countdown Mode
**Functionality**: Counts downward from user-defined time to 00:00:00
**Characteristics**:
- User-configurable start time using adjustment buttons
- Yellow LED indicator (PD5) active
- Buzzer activation when reaching 00:00:00
- Automatic pause when countdown completes
- Timer decrements: Seconds → Minutes → Hours

### Mode Transition Logic
```c
// Mode toggle implementation
if (!(PINB & (1 << PB7))) {
    Flag_Countdown_Mode ^= 1;    // Toggle mode flag
    PORTD ^= (1 << PD5);         // Toggle yellow LED
    PORTD ^= (1 << PD4);         // Toggle red LED
}
```

## User Interface

### Button Configuration and Functions
| Button | Pin | Function | Resistor | Trigger |
|--------|-----|----------|----------|---------|
| Reset | PD2 (INT0) | Reset time to 00:00:00 | Internal Pull-up | Falling Edge |
| Pause | PD3 (INT1) | Pause timer operation | External Pull-down | Rising Edge |
| Resume | PB2 (INT2) | Resume timer from pause | Internal Pull-up | Falling Edge |
| Mode Toggle | PB7 | Switch increment/countdown | Internal Pull-up | Active Low |
| Hours +/- | PB1/PB0 | Adjust hours | Internal Pull-up | Active Low |
| Minutes +/- | PB4/PB3 | Adjust minutes | Internal Pull-up | Active Low |
| Seconds +/- | PB6/PB5 | Adjust seconds | Internal Pull-up | Active Low |

### Visual Feedback System
- **Mode Indicators**:
  - Red LED: Increment mode active
  - Yellow LED: Countdown mode active
- **State Indicators**:
  - Buzzer: Countdown completion alert
  - LED toggling: Mode switching confirmation

## Timer Configuration

### Timer1 CTC Mode Setup
```c
void Timer1_CTC_Init(void) {
    // Configure Timer1 in CTC mode
    TCCR1A = (1 << FOC1A) | (1 << FOC1B);           // Non-PWM mode
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12); // CTC mode, 1024 prescaler
    
    // Timer calculations for 1-second interval
    // Fcpu = 16MHz, Prescaler = 1024
    // Timer resolution = 63.81 μs
    // Compare value for 1 second = 15672
    TCNT1 = 0;          // Initial value
    OCR1A = 15672;      // Compare value for 1-second interval
    
    TIMSK |= (1 << OCIE1A); // Enable Timer1 compare interrupt
}
```

### Timing Precision
- **System Frequency**: 16MHz
- **Prescaler**: 1024 (F_CPU/1024)
- **Timer Resolution**: 63.81 μs per tick
- **Compare Value**: 15672 for 1.00003032 second interval
- **Accuracy**: ±0.003% timing precision

## Display System

### Seven-Segment Multiplexing
```c
void Digit_And_Position(unsigned char Digit, unsigned char Position) {
    // Activate specific digit position
    PORTA = (PORTA & 0xC0) | (1 << Position);
    
    // Set BCD data for 7447 decoder
    PORTC = (PORTC & 0xF0) | (Digit & 0x0F);
}

void Time(void) {
    // Display all six digits with multiplexing
    Digit_And_Position(Secs % 10, 5);  _delay_ms(1);  // Seconds units
    Digit_And_Position(Secs / 10, 4);  _delay_ms(1);  // Seconds tens
    Digit_And_Position(Mins % 10, 3);  _delay_ms(1);  // Minutes units
    Digit_And_Position(Mins / 10, 2);  _delay_ms(1);  // Minutes tens
    Digit_And_Position(Hrs % 10, 1);   _delay_ms(1);  // Hours units
    Digit_And_Position(Hrs / 10, 0);   _delay_ms(1);  // Hours tens
}
```

### Display Characteristics
- **Refresh Rate**: ~165 Hz per digit (6ms total cycle)
- **Persistence of Vision**: Appears as solid display due to rapid switching
- **Digit Control**: PORTA (PA0-PA5) for individual digit enable
- **Data Interface**: PORTC (PC0-PC3) for BCD data to 7447 decoder

## Interrupt Handling

### External Interrupt Configuration

#### INT0 - Reset Function (Falling Edge)
```c
ISR(INT0_vect) {
    TCNT1 = 0;  // Reset timer counter
    Secs = 0;   // Reset seconds
    Mins = 0;   // Reset minutes  
    Hrs = 0;    // Reset hours
}
```

#### INT1 - Pause Function (Rising Edge)
```c
ISR(INT1_vect) {
    TCCR1B &= 0xF8;  // Stop timer by clearing prescaler bits
    Flag_Pause = 1;  // Set pause flag for main loop processing
}
```

#### INT2 - Resume Function (Falling Edge)
```c
ISR(INT2_vect) {
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Restart timer
    PORTD &= ~(1 << PD0);  // Turn off buzzer
    Flag_Pause = 0;        // Clear pause flag
}
```

### Timer1 Compare Interrupt
```c
ISR(TIMER1_COMPA_vect) {
    if (Flag_Countdown_Mode) {
        // Countdown logic with underflow handling
        if (Secs == 0) {
            if (Mins == 0) {
                if (Hrs == 0) {
                    PORTD |= (1 << PD0);  // Activate buzzer
                    return;                // Stop counting
                }
                Hrs--; Mins = 59; Secs = 59;
            } else {
                Mins--; Secs = 59;
            }
        } else {
            Secs--;
        }
    } else {
        // Increment logic with overflow handling
        PORTD |= (1 << PD4);   // Red LED on
        PORTD &= ~(1 << PD0);  // Buzzer off
        Secs++;
        if (Secs >= 60) {
            Secs = 0; Mins++;
            if (Mins >= 60) {
                Mins = 0; Hrs++;
                if (Hrs >= 24) {
                    Hrs = 0;  // 24-hour rollover
                }
            }
        }
    }
}
```

## Control Logic

### State Machine Implementation
The system operates as a state machine with the following states:
1. **Running Increment**: Default state, counting upward
2. **Running Countdown**: Counting downward from set time
3. **Paused**: Timer stopped, allowing time adjustment
4. **Countdown Complete**: Buzzer active, awaiting user action

### Button Debouncing
```c
// Debouncing implementation for all buttons
if (!(PINB & BUTTON_MASK)) {
    _delay_ms(30);                    // Debounce delay
    if (!(PINB & BUTTON_MASK)) {      // Confirm button press
        // Execute button function
        while(!(PINB & BUTTON_MASK)) { // Wait for release
            Time();                    // Continue display update
        }
    }
}
```

### Time Adjustment Logic
- **Hours**: 0-23 range with wraparound
- **Minutes**: 0-59 range with wraparound  
- **Seconds**: 0-59 range with wraparound
- **Adjustment**: Only available in paused state
- **Validation**: Automatic range checking and boundary handling

## Hardware Connections

### Port Assignments
```c
// PORTA: Seven-segment digit enable (PA0-PA5)
DDRA |= 0x3F;    // Configure as output

// PORTB: Button inputs with internal pull-ups
DDRB = 0x00;     // Configure as input
PORTB = 0xFF;    // Enable pull-ups

// PORTC: BCD data to 7447 decoder (PC0-PC3)
DDRC |= 0x0F;    // Configure as output
PORTC &= 0xF0;   // Initialize to zero

// PORTD: Mixed I/O
// PD0 (Output): Buzzer
// PD2 (Input): Reset button (INT0)
// PD3 (Input): Pause button (INT1) 
// PD4 (Output): Red LED
// PD5 (Output): Yellow LED
```

### Circuit Integration
- **7447 Decoder**: Converts 4-bit BCD to seven-segment patterns
- **Transistor Switching**: NPN BJT transistors control digit enable/disable
- **Pull-up Resistors**: Internal pull-ups for most buttons, external for INT1
- **LED Current Limiting**: Appropriate resistors for LED indicators
- **Buzzer Driver**: Direct drive capability or transistor amplification

## Software Implementation

### Global Variables
```c
unsigned char Hrs = 0;                 // Hours counter (0-23)
unsigned char Mins = 0;                // Minutes counter (0-59)
unsigned char Secs = 0;                // Seconds counter (0-59)
unsigned char Flag_Countdown_Mode = 0; // Mode flag: 0=increment, 1=countdown
unsigned char Flag_Pause = 0;          // Pause state flag
```

### Main Program Flow
1. **Initialization**: Configure ports, timers, and interrupts
2. **Display Loop**: Continuous seven-segment multiplexing
3. **Mode Detection**: Check for mode toggle button
4. **Pause Handling**: Time adjustment when paused
5. **State Management**: Handle flags and user interactions

### Memory Usage
- **Flash**: Efficient code implementation with minimal memory footprint
- **SRAM**: Global variables and stack usage optimized
- **Registers**: Direct register manipulation for performance

## Development Environment

**IDE**: Eclipse IDE for AVR Development  
**Simulation**: Proteus Design Suite for circuit simulation and testing  
**Programmer**: USBasp or similar AVR programmer  
**Compiler**: AVR-GCC with optimization settings  

## Operation Procedure

### Initial Setup
1. **Power On**: System starts in increment mode with red LED active
2. **Timer Start**: Automatic counting begins from 00:00:00
3. **Display Active**: Six seven-segment displays show current time

### Mode Switching
1. **Pause Timer**: Press pause button (PD3) to stop counting
2. **Toggle Mode**: Press mode button (PB7) to switch increment/countdown
3. **Set Time**: Use adjustment buttons to set desired countdown time
4. **Resume**: Press resume button (PB2) to start countdown

### Time Adjustment (Pause Mode Only)
- **Hours Adjustment**: PB1 (increment), PB0 (decrement)
- **Minutes Adjustment**: PB4 (increment), PB3 (decrement)
- **Seconds Adjustment**: PB6 (increment), PB5 (decrement)

### Reset Operation
- **Any Mode**: Press reset button (PD2) to return to 00:00:00
- **Immediate**: Reset occurs instantly regardless of current state

## Project Structure

```
Dual_Mode_Stopwatch/
├── src/
│   ├── Stopwatch_With_Dual_Mode.c     # Main application logic
│   ├── Functions_Definitions.c        # Function implementations  
│   └── Functions_Prototypes.h         # Header file with declarations
├── Documentation/
│   ├── Mini_Project2.pdf              # Project specifications
│   └── README.md                       # This file
├── Simulation/
│   ├── Proteus_Project.pdsprj         # Proteus simulation file
│   └── Circuit_Diagram.png            # Hardware schematic
└── Demo/
    └── Operation_Video.mp4             # System demonstration
```

## License

This is an educational project developed as part of the Standard Embedded Systems Diploma program.

---

**Developed by**: Bassam Ashraf  
**Course**: Standard Embedded Systems Diploma  
**Program**: EDGES Training Program  
**Development Environment**: Eclipse IDE, Proteus Design Suite  
**Language**: C  

---

*This dual-mode stopwatch system demonstrates advanced embedded systems programming including timer configuration, interrupt handling, seven-segment multiplexing, and real-time user interface management. The project showcases professional embedded software development practices with emphasis on precise timing control and responsive user interaction.*
