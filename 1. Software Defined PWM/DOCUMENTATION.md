#**Lab 4 part1.c**
#### Keiane Balicanta
## Purpose
The purpose of this lab is to utilize PWM and the MSP430s dedicated timers to change the "brightness" of the on-board LEDs.

## Psuedocode
The basic pseudocode for part1.c is as follows:
```
    include <msp430.h>
    function prototypes

    int main(){
        stop watchdog
        configure GPIO

        LEDSetup();
        ButtonSetup();
        Timer1Setup();
        Timer2Setup();
    }

    void LEDSetup(){ }
    void ButtonSetup(){ }
    void Timer1Setup(){ Using TB0CCR0 and TB0CCR1 }
    void Timer2Setup(){ Using TB1CCR0 and TB0CCR1 }

    #pragma vector = TIMER B0 CCR0 setup
    #pragma vector = TIMER B0 CCR1 setup

    #pragma vector = TIMER B1 CCR0 setup
    #pragma vector = TIMER B1 CCR1 setup
```
## Main Function
* The purpose of the main function is to strictly call the setup functions and enable interrupts. There is no while(1) loop this time to emphasize the power saving methods that timers and interrupts provide compared to polling and __delay_cycles().

* **Lines 22 and 26** are meant to stop the watchdog timer and configure GPIO ports respectively:
```
    WDTCTL = WDTPW | WDTHOLD;
    ...
    ...
    ...
    PM5CTL0 &= ~LOCKLPM5;
```

* And **Lines 29-32** are the function calls:
```    
    LEDSetup();
    ButtonSetup();
    TimerB0Setup();
    TimerB1Setup();
```
* Finally, interrupt enabled is found in line 34:
```
    __bis_SR_register(LPM0_bits | GIE);
```

## LED Initializations
* Using bitmasking, clearing, and setting bits, port 1 using pin 0 and port 6 using pin 6 were set to outputs and cleared. These are notably the on-board LEDs which we will be using to be controlled by the buttons and interrupts.

* **Lines 39-43** set up these LEDs:
```
    P1OUT &= ~BIT0;                           
    P1DIR |= BIT0;                            

    P6OUT &= ~BIT6;                  
    P6DIR |= BIT6;                
```
## Button Initializations
* Because the buttons are being read as inputs, the process is similar but different from initializing the LEDs above. The buttons being used are the on-board buttons, P2.3 and P4.1. Setting BIT3 of the P2OUT and P2REN configures the 3rd bit of the P2OUT register as an input with a pulled-up resistor enabled. 

* P2IES and P2IE enables the interrupts of the port 2 vectors to be used later and sets the direction bit from HIGH -> LOW. This means that the interrupt will fire as soon as the signal goes from 1 to 0 or as soon as the button is pressed. A LOW -> HIGH direction will fire the interrupt only when the signal goes from 0 to 1 or the button is pressed but then released.

* **Lines 47-50**:
```
    P2OUT |= BIT3;
    P2REN |= BIT3;
    P2IES |= BIT3;
    P2IE |= BIT3;
```

* Repeat this process for button 4.1 in **Lines 52-55**:
```
    P4OUT |= BIT1;
    P4REN |= BIT1;
    P4IES |= BIT1;
    P4IE |= BIT1;
```

## Timer B0 Setup
*  The plan for setting up the timer is to use a 1kHz signal and a 1ms period. This means the time from one blink to the next is 1ms. **ACLK** is going to be used in **UP** mode.

* Period Setup with ACLK in UP Mode, **Line 71:**
    ```
        TB0CTL = TBSSEL_2 | MC_1;
    ```
* TB0 refers to TIMER0 on board. There are 3 timers, TIMER0, TIMER1, and TIMER2. Each timer has some amount of capture compare registers **(CCRx)**. Each timer has a CCR0 which is specifically meant to be the count number that the binary counter counts up to. Reffering to an equation, **t = T*N** where t is time, T is the period, and N is the binary counts, CCR0 is N.
*  For a 1ms time, in UP mode, the equation would be: **0.001 = (1/32.768k)(N)**

## Button Interrupts
*  Each time **button 2.3** is pressed, it needs to correspond to the speed of the **green LED**. Conversely, eache time **button 4.1** is pressed, ot needs to correspond to the speed of the **red LED**.
* Interrupt service routines are implemented to handle these button presses in lines 87-99 for button 2.3 and lines 101-114. Everytime the button is pressed, it increments its corresponding capture control register **(CCRx)** by 100. Therefore, the time on vs the time off is increased. When the time on reaches the max value of 1000, it reverts back to 0. Basically, when the LED reaches its brightest point, it goes back to its lowest point and starts incrementing again.

* Lines 117-149 show the vector initializations for the timer interrupts. As mentioned earlier, timer B0 handles the red LED's timing and timer B1 handles the green LED's timing through bit set and bit clear instructions.

