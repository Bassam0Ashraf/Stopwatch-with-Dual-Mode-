# **Project Overview:**

This project involves creating a versatile stopwatch that operates in two distinct modes using ATmega32 :

1. **Increment Mode (Default):** The stopwatch counts upwards from zero, displaying hours,
minutes, and seconds on six seven-segment displays.

2. **Countdown Mode:** The stopwatch counts downwards from a user-defined time,
functioning as a countdown timer

# **Features:**

1. **Reset Function:** Resets the time to zero in both modes.

2. **Pause Function:** Pauses the time counting, allowing the stopwatch to hold its current value.

3. **Resume Function:** Resumes counting from the paused time.

4. **Mode Toggle:** Switches between increment and countdown modes.


# **Key Hardware & Software Components:**
1. **Microcontroller:** ATmega32

2. **7-Segment Display:** Used to display the current time (hours, minutes, and
seconds).

3. **LED Indicators:** To indicate the current mode (increment or countdown).

4. **Buzzer:** Activated when the countdown reaches zero.

5. **Push Buttons:** For adjusting time and toggling modes.

# **Operation Description:**
- **Increment Mode:** By default, the stopwatch starts in increment mode, where the time
counts up continuously from zero. The time is displayed across six seven-segment displays,
showing hours, minutes, and seconds. The red LED is turned on to indicate increment mode.

- **Countdown Mode:** When the mode toggle button is pressed, the stopwatch switches to
countdown mode. In this mode, the user can set the desired countdown time using the
hour, minute, and second adjustment buttons. The countdown begins once the
start/resume button is pressed. A yellow LED on is turned on to indicate countdown
mode. An alarm is triggered when the countdown reaches zero.
