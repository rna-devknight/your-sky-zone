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

void config_ACLK_to_32KHz_crystal(void);
void Initialize_UART(void);
void uart_write_char(unsigned char ch);
void handleUARTstring(void);
unsigned char uart_read_char(void);
void uart_write_string(char * str);
char *takeUARTinput(void);
void Initialize_LCD(void);
void display_uint16_LCD(unsigned int num);
void display_digit(int num, int digit);
void display_char(char letter, int digit);
void display_string(char *string);

// The array has the shapes of the digits (0 to 9)
const unsigned char LCD_Num[10] = {
    0xFC, 0x60, 0xDB, 0xF1, 0x67,
    0xB7, 0xBF, 0x80, 0xFF, 0xE7};
const unsigned char LCD_Num2[10] = {
    0x00, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x30, 0x00, 0x00};
const unsigned char LCD_Alpha[26] = {
    0xEF, 0xF3, 0x9C, 0xF0, 0x9F, 0x8F, 0xBD, 0x6F, 0x90,
    0x78, 0x0E, 0x1C, 0x6C, 0x6C, 0xFC, 0xCF, 0xFC, 0xCF,
    0xB1, 0x80, 0x7C, 0x0C, 0x6C, 0x00, 0x00, 0x90};
const unsigned char LCD_Alpha2[26] = {
    0x00, 0x50, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x50,
    0x00, 0x22, 0x00, 0xA0, 0x82, 0x00, 0x00, 0x02, 0x02,
    0x80, 0x50, 0x00, 0x28, 0x0A, 0xAA, 0xA8, 0x28};

volatile int counterState = 0;
volatile unsigned int seconds = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;     // Enable GPIO pins

    // set the pin directions to output
    P1DIR |= redLED;
    P9DIR |= greenLED;

    P1OUT &= ~redLED;
    P9OUT &= ~greenLED;

    // Initializes the LCD_C module
    Initialize_LCD();

    Initialize_UART();

    handleUARTstring();

    return 0;
}

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal(void)
{
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz

    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do
    {
        CSCTL5 &= ~LFXTOFFG; // Local fault flag
        SFRIFG1 &= ~OFIFG;   // Global fault flag
    } while ((CSCTL5 & LFXTOFFG) != 0);

    CSCTL0_H = 0; // Lock CS registers
    return;
}

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

// Function to transmit a string of any length
void uart_write_string(char *str)
{
    volatile uint32_t i = 0, len = strlen(str);

    for (i = 0; i < len; i++)
        uart_write_char(str[i]);

    return;
}

// void uart_write_string(char * str)
// {
//     int i;
//     for (i = 0; str[i] != '\0'; i++)
//     {
//         uart_write_char(str[i]);
//     }
// }

// char *takeUARTinput(void)
// {
//     volatile char i, in;
//     volatile unsigned int j;

//     // char debugString[] = "See here: ";
//     char *stringFromUART = "";
//     char chToStr[2];

//     // Initialize chToStr to end with a null terminating character
//     chToStr[1] = '\0';

//     for (i = '0'; i <= '7';)
//     {
//         P1OUT ^= redLED;
//         // uart_write_string("Begin Iteration ");
//         uart_write_char(i);
//         uart_write_string(": ");

//         for (j = 0; j < 50000; j++)
//         {
//             in = toupper(uart_read_char());

//             // If the input is a digit between 0 - 9 or A - Z
//             if ((in >= '0' && in <= '9') || (in >= 'A' && in <= 'Z'))
//             {
//                 // P9OUT |= greenLED;
//                 uart_write_char(in);
//                 uart_write_string(" ");
//                 chToStr[0] = in;
//                 strcat(stringFromUART, chToStr);
//                 i++;
//                 break;
//             }
//         }

//         uart_write_string(stringFromUART);
//         uart_write_string("1234567");
//         // uart_write_string("End Iteration");
//         // uart_write_string(" ");
//         uart_write_char('\n');
//         uart_write_char('\r');
//     }

//     return stringFromUART;
// }

int validCharacter(char input)
{
    if (input >= 'a' && input <= 'z')
        return 1;
    else if (input >= 'A' && input <= 'Z')
        return 1;
    else if (input >= '0' && input <= '9')
        return 1;
    else
        return 0;
}

void handleUARTstring(void)
{
    char finalString[17] = "";
    char *temp;

    volatile char i, input;
    volatile unsigned int j;

    int length = 0;

    // writing uart output for debug purposes
    // constantly pinging the uart RX lines for any data
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

                // if valid character then add to string
                if (validCharacter(input))
                {
                    temp = malloc(sizeof(char));
                    sprintf(temp, "%c", toupper(input));
                    strcat(finalString, temp);
                    free(temp);
                }
                else if (input == '!') // designated termination character
                {
                    display_string(finalString);
                    strcpy(finalString, "");
                }
            }
        }
    }
}

//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989’s Sample Code ***
void Initialize_LCD(void)
{
    PJSEL0 = BIT4 | BIT5;       // For LFXT

    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;    // Enable LFXT
    do
    {
        CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG); // Test oscillator fault flag
    CSCTL0_H = 0;              // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM; // Clear LCD memory

    // Turn LCD on
    LCDCCTL0 |= LCDON;

    return;
}

void display_uint16_LCD(unsigned int num)
{
    // unsigned int n = num;
    int currDigit = 0, i = 0;

    do
    {
        currDigit = num % 10;
        display_digit(currDigit, i);
        num /= 10;
        i++;
    } while (num > 0);

    return;
}

void display_digit(int num, int digit)
{
    // LCDMx corresponding to each place (we want right justified)
    // LCDM10, LCDM6, LCDM4, LCDM19, LCDM15, LCDM8
    switch (digit)
    {
    case 0:
        LCDM8 = LCD_Num[num];
        break;
    case 1:
        LCDM15 = LCD_Num[num];
        break;
    case 2:
        LCDM19 = LCD_Num[num];
        break;
    case 3:
        LCDM4 = LCD_Num[num];
        break;
    case 4:
        LCDM6 = LCD_Num[num];
        break;
    case 5:
        LCDM10 = LCD_Num[num];
        break;
    default:
        break;
    }

    return;
}

void display_char(char input, int digit)
{
    if (input >= 'A' && input <= 'Z')
    {
        int letterIndex = input - 'A';
        switch (digit)
        {
        case 0:
            LCDM8 = LCD_Alpha[letterIndex];
            LCDM9 = LCD_Alpha2[letterIndex];
            break;
        case 1:
            LCDM15 = LCD_Alpha[letterIndex];
            LCDM16 = LCD_Alpha2[letterIndex];
            break;
        case 2:
            LCDM19 = LCD_Alpha[letterIndex];
            LCDM20 = LCD_Alpha2[letterIndex];
            break;
        case 3:
            LCDM4 = LCD_Alpha[letterIndex];
            LCDM5 = LCD_Alpha2[letterIndex];
            break;
        case 4:
            LCDM6 = LCD_Alpha[letterIndex];
            LCDM7 = LCD_Alpha2[letterIndex];
            break;
        case 5:
            LCDM10 = LCD_Alpha[letterIndex];
            LCDM11 = LCD_Alpha2[letterIndex];
            break;
        default:
            break;
        }
    }
    else if (input >= '0' && input <= '9')
    {
        int numberIndex = input - '0';
        switch (digit)
        {
        case 0:
            LCDM8 = LCD_Num[numberIndex];
            LCDM9 = LCD_Num2[numberIndex];
            break;
        case 1:
            LCDM15 = LCD_Num[numberIndex];
            LCDM16 = LCD_Num2[numberIndex];
            break;
        case 2:
            LCDM19 = LCD_Num[numberIndex];
            LCDM20 = LCD_Num2[numberIndex];
            break;
        case 3:
            LCDM4 = LCD_Num[numberIndex];
            LCDM5 = LCD_Num2[numberIndex];
            break;
        case 4:
            LCDM6 = LCD_Num[numberIndex];
            LCDM7 = LCD_Num2[numberIndex];
            break;
        case 5:
            LCDM10 = LCD_Num[numberIndex];
            LCDM11 = LCD_Num2[numberIndex];
            break;
        default:
            break;
        }
    }
}

void display_string(char *string)
{
    int i, j, charCount = 0;
    int indexShift, remainingChars;
    volatile unsigned long int counter;

    // this function accepts uppercase only

    for (i = 0; string[i] != '\0'; i++)
    {
        charCount++;
    }

    if (charCount > 6)
        for (i = 0; i < charCount; i++)
        {
            indexShift = 0;
            remainingChars = charCount - i;

            if (remainingChars > 6)
            {
                // shift all characters to the left one position
                for (j = i; j < i + 6; j++)
                {
                    display_char(string[j], 5 - indexShift);
                    indexShift++;
                }
            }
            else
            {
                for (j = i; j < charCount; j++)
                {
                    display_char(string[j], 5 - indexShift);
                    indexShift++;
                }
            }
            // delay counter
            for (counter = 0; counter < 50000; counter++);
            LCDCMEMCTL = LCDCLRM; // Clears all the segments

            // // try to use precise 1 second counter
            // TA0CTL &= ~TAIFG;
            // while (TA0CTL & TAIFG == 0);

            // LCDCMEMCTL = LCDCLRM;    // Clears all the segments
    }
    else
        for (i = 0; i < charCount; i++)
            display_char(string[i], (charCount - 1) - i);
}
