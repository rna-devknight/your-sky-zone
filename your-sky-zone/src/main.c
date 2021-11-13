#include <msp430fr6989.h>
#define redLED BIT0
#define greenLED BIT7
#define BUT1 BIT1
#define BUT2 BIT2

void config_ACLK_to_32KHz_crystal()
{

  PJSEL1 &= ~BIT4;
  PJSEL0 |= BIT4;

  CSCTL0 = CSKEY;

  do
  {
    CSCTL5 &= ~LFXTOFFG;
    SFRIFG1 &= ~OFIFG;
  } while ((CSCTL5 & LFXTOFFG) != 0);

  CSCTL0_H = 0;
  return;
}

/**
 * main.c
 */
int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;
  PM5CTL0 &= ~LOCKLPM5;
  P1DIR &= ~(BUT1 | BUT2);
  P1REN |= (BUT1 | BUT2);
  P1OUT |= (BUT1 | BUT2);
  P1IE |= (BUT1 | BUT2);
  P1IES |= (BUT1 | BUT2);
  P1IFG &= ~(BUT1 | BUT2);

  P1DIR |= redLED;
  P9DIR |= greenLED;
  P1OUT &= ~redLED;
  P9OUT &= ~greenLED;
  config_ACLK_to_32KHz_crystal();
  TA0CTL &= ~TAIFG;
  _low_power_mode_3();

  return 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR()
{
  if ((P1IN & BUT2) == 0)
  {
    TA0CCR0 = (16384 - 1) / 10;
    TA0CTL = TASSEL_1 | ID_2 | MC_1 | TACLR | TAIE;
    TA0CTL &= ~TAIFG;
    P1IE &= ~BUT2;
    P1IFG &= ~BUT2;
  }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void T0A1_ISR()
{
  P9OUT ^= greenLED;
  TA0CTL &= ~TAIFG;
  P1IE |= BUT2;
  P1IFG &= ~BUT2;
  TA0CTL &= ~TAIE;
}