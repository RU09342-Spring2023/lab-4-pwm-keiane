#**Lab 4 part2.c**
#### Keiane Balicanta

## Purpose
The purpose of this lab is to utilize hardware PWM and the MSP430s dedicated output modes to control an external LED.

## Main Function and Timers
* The purpose of the main function is to strictly call the setup functions for the LEDs and timers. Definitions for the different fades were included in the beginning of the code. The requirements for the fades are as follows:
    1. Red
    2. Orange (Red + Green)
    3. Green
    4. Cyan (Green + Blue)
    5. Blue
    6. Purple (Red + Blue)

* **Lines 38-53** setup the multiple capture control registers and utilize the output mode 7 (set/reset) initializations. More specifically, **lines 51-53** set the interrupt enables for timer B1. This utilizes ACLK in CONTINUOUS mode with the clock divided by 4 followed by TBCLR and TBIE instructions for clearing and enabling interrupts.  
```
TB1CCTL0 |= CCIE;
TB1CCR0 = 1;
TB1CTL = TBSSEL_1 | MC_2 | ID_3 | TBCLR | TBIE;
```

## LED Initializations
* Since we are using an externing RGB LED, we have to use output pins. In the `void LED()` function from **Lines 56-74**, are using P6.0, P6.1, and P6.2 as output pins.
```
void LED(){
    P6DIR |= BIT0;                     // P6.0 output
    P6OUT &= ~BIT0;
    P6SEL0 |= BIT0;
    P6SEL1 &= ~BIT0;                    // P6.0 options select
    P6IE |= BIT0;

    P6DIR |= BIT1;                     // P6.1 output
    P6OUT &= ~BIT1;
    P6SEL0 |= BIT1;
    P6SEL1 &= ~BIT1;                    // P6.1 options select
    P6IE |= BIT1;

    P6DIR |= BIT2;                     // P6.2 output
    P6OUT &= ~BIT2;
    P6SEL0 |= BIT2;
    P6SEL1 &= ~BIT2;                    // P6.2 options select
    P6IE |= BIT2;
}
```

## Interrupts
* **Lines 76-109** show a switch statement representing the RGB fade from the aforementioned color fades. An example shown below represents 1 case: 
```
case RED_GREEN:
            TB3CCR1--;
            TB3CCR2++;
            if(TB3CCR1 == 0){
                state = GREEN_BLUE;
            }
            break;
```
We see that the red LED (CCR1) starts decreasing by 1 from 1000 (as initialized in Line 43) and the green LED (CCR2) starts incrementing thus showing the RED to GREEN fade also creating a subtle cyan. Once the RED LED is fully dimmed, the `state` variable enters to the `GREEN_BLUE` defined state which then just repeats this process.