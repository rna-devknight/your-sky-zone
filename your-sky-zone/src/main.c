#include <msp430fr6989.h>

#define FLAGS       UCA1IFG     // Contains the transmit & receive flags
#define RXFLAG      UCRXIFG         // Receive flag
#define TXFLAG      UCTXIFG         // Transmit flag
#define TXBUFFER    UCA1TXBUF       // Transmit buffer
#define RXBUFFER    UCA1RXBUF       // Receive buffer
#define redLED      BIT0            // Red at P1.0
#define greenLED    BIT7            // Green at P9.7

// Configure UART to the popular configuration
// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
// no flow control
// Initial clock: SMCLK @ 1.048 MHz with oversampling
void Initialize_UART(void)
{
    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4 | BIT5);
    P3SEL0 |= (BIT4 | BIT5);

    // Use SMCLK clock; leave other settings default
    UCA1CTLW0 |= UCSSEL_3;

    // Configure the clock dividers and modulators
    // UCBR=6, UCBRF=13, UCBRS=0x22, UCOS16=1 (oversampling)
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS5 | UCBRS1 | UCBRF3 | UCBRF2 | UCBRF0 | UCOS16;

    // Exit the reset state (so transmission/reception can begin)
    UCA1CTLW0 &= ~UCSWRST;
}

void uart_write_char(unsigned char ch)
{
    // Wait for any ongoing transmission to complete
    while ((FLAGS & TXFLAG) == 0) {}

    // Write the byte to the transmit buffer
    TXBUFFER = ch;
}

// The function returns the byte; if none received, returns NULL
unsigned char uart_read_char(void)
{
    unsigned char temp;

    // Return NULL if no byte received
    if ((FLAGS & RXFLAG) == 0)
        return '\0';

    // Otherwise, copy the received byte (clears the flag) and return it
    temp = RXBUFFER;
    
    return temp;
}

int main(void)
{
  // volatile unsigned int i;
  WDTCTL = WDTPW | WDTHOLD;   // Stop the Watchdog timer
  PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode

  // P1DIR |= redLED;            // Direct pin as output
  // P1OUT &= ~redLED;           // Turn LED off

  // for(;;)
  // {
  //   // Delay loop
  //   for(i = 0; i < 50000; i++);

  //   P1OUT ^= redLED;        // Toggle the LED
  // }

  // set the pin directions to output
  P1DIR |= redLED;
  P9DIR |= greenLED;

  P1OUT &= ~redLED;
  P9OUT &= ~greenLED;

  Initialize_UART();
  volatile char i, input;
  volatile unsigned int j;


  for (;;)
  {
      for (i = '0'; i <= '9'; i++)
      {
          uart_write_char(i);
          uart_write_char('\n');
          uart_write_char('\r');

          P1OUT ^= redLED;

          for (j = 0; j < 50000; j++)
          {
              input = uart_read_char();

              if (input == '1')
              {
                  P9OUT |= greenLED;
              }
              else if (input == '2')
              {
                  P9OUT &= ~greenLED;
              }
          }
      }
  }

  return 0;
}