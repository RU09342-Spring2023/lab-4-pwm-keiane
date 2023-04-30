/*
 * HardwarePWMExample.c
 *
 *  Created on: Feb 18, 2023
 *      Author: Russell Trafford
 *
 *      This example configures P6.0 as a PWM output as controlled by TimerB3.
 *
 *      You will need to connect an external LED or use a scope to see the effect of the Duty Cycle.
 *
 *      You can control the DutyCycle by changing the values in the TB3CCR1 register
 */

#include <msp430.h>
#define RED_GREEN 0
#define GREEN_BLUE 1
#define BLUE_RED 2


void LED();
void timer();

char state = RED_GREEN;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT
    timer();
    LED();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    __bis_SR_register(LPM0_bits | GIE);             // Enter LPM0
    __no_operation();                         // For debugger
}

void timer(){
    TB3CCR0 = 1000-1;                         // PWM Period
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;  // SMCLK, up mode, clear TBR

    TB3CCTL1 = OUTMOD_7;                      // CCR1 reset/set
    TB3CCR1 = 1000-1;                            // CCR1 PWM duty cycle

    TB3CCTL2 = OUTMOD_7;                      // CCR1 reset/set
    TB3CCR2 = 0;                            // CCR1 PWM duty cycle

    TB3CCTL3 = OUTMOD_7;                      // CCR1 reset/set
    TB3CCR3 = 0;                            // CCR1 PWM duty cycle

    TB1CCTL0 |= CCIE;
    TB1CCR0 = 1;
    TB1CTL = TBSSEL_1 | MC_2 | ID_3 | TBCLR | TBIE;
}

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

#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void){

    switch(state){
        case RED_GREEN:
            TB3CCR1--;
            TB3CCR2++;
            if(TB3CCR1 == 0){
                state = GREEN_BLUE;
            }
            break;
        case GREEN_BLUE:
            TB3CCR2--;
            TB3CCR3++;
            if(TB3CCR2 == 0){
                state = BLUE_RED;
            }
            break;
        case BLUE_RED:
            TB3CCR3--;
            TB3CCR1++;
            if(TB3CCR3 == 0){
                state = RED_GREEN;
            }
            break;
    }
    if (TB3R >= 60000){
        TB3R = 1;
    }
    TB3CCR1 = TB3CCR1;
    TB3CCR2 = TB3CCR2;
    TB3CCR3 = TB3CCR3;
    TB1CCR0 += 200;
}




