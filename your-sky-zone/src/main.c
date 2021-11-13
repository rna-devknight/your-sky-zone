#include <msp430fr6989.h>
#define redLED BIT0

int main(void)
{
  volatile unsigned int i;
  WDTCTL = WDTPW | WDTHOLD;   // Stop the Watchdog timer
  PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode

  P1DIR |= redLED;            // Direct pin as output
  P1OUT &= ~redLED;           // Turn LED off

      for(;;) {
        // Delay loop
        for(i = 0; i < 50000; i++);

        P1OUT ^= redLED;        // Toggle the LED
    }

  return 0;
}