#include <msp430fr6989.h>

#include <stdint.h>
#define redLED BIT0
#define greenLED BIT7

void main(void)
{

  volatile unsigned int i;
  volatile uint32_t j;
  WDTCTL = WDTPW | WDTHOLD;
  PM5CTL0 &= ~LOCKLPM5;

  P1DIR |= redLED;
  P1OUT &= ~redLED;

  P9DIR |= greenLED;
  P9OUT &= ~greenLED;

  for (;;)
  {
    // Delay loop
    for (i = 0; i < 20000; i++)
    {
    }

    P1OUT ^= redLED;

    for (i = 0; i < 20000; i++)
    {
    }

    P1OUT ^= redLED;

    for (i = 0; i < 20000; i++)
    {
    }

    P9OUT ^= greenLED;
  }
}
