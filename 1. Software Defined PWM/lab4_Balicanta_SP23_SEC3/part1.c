/*
 * part1.c
 *
 *  Created on: Feb 22, 2023
 *      Author: kbali
 *
 *  Purpose: This file will configure button 2.3 to change the duty cycle of the green LED by increasing
 *  it 10% upon each press. Button 4.1 will do the same thing but by corresponding with the red LED.
 */

#include <msp430.h>


// ==== Prototypes ==== //
void LEDSetup();
void ButtonSetup();
void TimerB0Setup();
void TimerB1Setup();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                     // Stop WDT

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // ==== Setup Function Calls ==== //
    LEDSetup();
    ButtonSetup();
    TimerB0Setup();
    TimerB1Setup();

    __bis_SR_register(LPM0_bits | GIE);          // Enter LPM0 w/ interrupt
}

void LEDSetup(){
    // Configure GPIO
    P1OUT &= ~BIT0;                                // P1.0 clear to 0
    P1DIR |= BIT0;                                // P1.0 output

    P6OUT &= ~BIT6;                                // P6.6 clear to 0
    P6DIR |= BIT6;                                // P6.6 output
}

void ButtonSetup(){
    P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
    P2REN |= BIT3;                          // P2.3 pull-up register enable
    P2IES |= BIT3;                         // P2.3 High --> Low edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    P4OUT |= BIT1;                          // Configure P2.3 as pulled-up
    P4REN |= BIT1;                          // P2.3 pull-up register enable
    P4IES |= BIT1;                         // P2.3 High --> Low edge
    P4IE |= BIT1;                           // P2.3 interrupt enabled
}

void TimerB0Setup(){
    // PERIOD
    TB0CTL = TBSSEL_2 | MC_1;               // SMCLK, up mode
    TB0CCTL0 |= CCIE;                             // TBCCR0 capture control interrupt enabled
    TB0CCR0 = 1000 - 1;                            // set TB0 capture compare register 0 count to 999 -> this is the PWM freq = (ACLK freq) / (CCR0)
    // DUTY CYCLE
    TB0CCTL1 |= CCIE;                             // TBCCR0 interrupt enabled
    TB0CCR1 = 500;                            // set TB0 capture compare register 1 count (the duty cycle) to 50% of period
}

void TimerB1Setup(){

    // PERIOD
    TB1CTL = TBSSEL_2 | MC_1;    // ACLK, up mode, interrupt enable, clear
    TB1CCTL0 |= CCIE;                             // TBCCR0 interrupt enabled
    TB1CCR0 = 1000 - 1;                            // set TB1 capture compare register 0 count to 65536

    // DUTY CYCLE
    TB1CCTL1 |= CCIE;                             // TBCCR1 interrupt enabled
    TB1CCR1 = 500;                            // set TB1 capture compare register 1 count to 50% of period
}

/*
 * INTERRUPT ROUTINES
 */

// ============= BUTTONS ISR =================================== //

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
        P2IFG &= ~BIT3;

    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.
        if ((P2IES & BIT3) && (TB1CCR1 <= 999)){
            TB1CCR1 += 100;
        } else {
            TB1CCR1 = 0;
        }
}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
        P4IFG &= ~BIT1;

    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.
        if ((P4IES & BIT1) && (TB0CCR1 <= 999)){
            TB0CCR1 += 100;
        } else {
            TB0CCR1 = 0;
        }
}


// ============= TIMER ISR =================================== //

// Timer B0 period cycle ISR
#pragma vector= TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
        P1OUT |= BIT0;      // set red LED when TB0CCR0 rolls over to 0
        TB0CCTL0 &= ~CCIFG;
}

// Timer B0 duty cycle ISR
#pragma vector= TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void)
{
        P1OUT &= ~BIT0;      // green LED turns off when TB0CCR1 reaches its value over time
        TB0CCTL1 &= ~CCIFG;
}

// Timer B1 period cycle ISR
#pragma vector= TIMER1_B0_VECTOR
__interrupt void ISR_TB1_CCR0(void)
{
        P6OUT |= BIT6;      // set green LED when TB1CCR0 rolls over to 0
        TB1CCTL0 &= ~CCIFG;
}

// Timer B1 duty cycle ISR
#pragma vector= TIMER1_B1_VECTOR
__interrupt void ISR_TB1_CCR1(void)
{
        P6OUT &= ~BIT6;      // green LED turns off when TB1CCR1 reaches its value over time
        TB1CCTL1 &= ~CCIFG;
}



