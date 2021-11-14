#include <msp430fr6989.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define FLAGS UCA1IFG      // Contains the transmit & receive flags
#define RXFLAG UCRXIFG     // Receive flag
#define TXFLAG UCTXIFG     // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer
#define redLED BIT0        // Red at P1.0
#define greenLED BIT7      // Green at P9.7

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
  while ((FLAGS & TXFLAG) == 0)
  {
  }

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

// Function to transmit a string of any length
void uart_write_string(char *str)
{
  volatile uint32_t i = 0, len = strlen(str);

  for (i = 0; i < len; i++)
    uart_write_char(str[i]);

  return;
}

char *takeUARTinput()
{
  volatile char i, in;
  volatile unsigned int j;

  // char debugString[] = "See here: ";
  char *stringFromUART = "";
  char chToStr[2];

  // Initialize chToStr to end with a null terminating character
  chToStr[1] = '\0';

  for (i = '0'; i <= '7';)
  {
    P1OUT ^= redLED;
    // uart_write_string("Begin Iteration ");
    uart_write_char(i);
    uart_write_string(": ");

    for (j = 0; j < 50000; j++)
    {

      in = toupper(uart_read_char());

      // If the input is a digit between 0 - 9 or A - Z
      if ((in >= 48 && in <= 57) || (in >= 65 && in <= 90))
      {
        // P9OUT |= greenLED;
        uart_write_char(in);
        uart_write_string(" ");
        chToStr[0] = in;
        strcat(stringFromUART, chToStr);
        i++;
        break;
      }
    }

    uart_write_string(stringFromUART);
    uart_write_string("1234567");
    // uart_write_string("End Iteration");
    // uart_write_string(" ");
    uart_write_char('\n');
    uart_write_char('\r');
  }

  return stringFromUART;
}

int main(void)
{
  // volatile unsigned int i;
  char *finalString = "";

  WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
  PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO power-on default high-impedance mode

  // set the pin directions to output
  P1DIR |= redLED;
  P9DIR |= greenLED;

  P1OUT &= ~redLED;
  P9OUT &= ~greenLED;

  Initialize_UART();
  volatile char i, input;
  volatile unsigned int j;

  finalString = takeUARTinput();

  uart_write_string("\nTHIS IS THE FINAL STRING\n");
  uart_write_string(finalString);
  uart_write_char('\n');
  uart_write_char('\r');

  for (;;)
  {
    uart_write_string(finalString);
    uart_write_char('\n');
    uart_write_char('\r');
  }

  return 0;
}