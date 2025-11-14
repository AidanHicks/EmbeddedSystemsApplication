#include <xc.h>
#define _XTAL_FREQ 16000000

// 7-segment patterns (common cathode)
const unsigned char SEGMENT_TABLE[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

#define MAX_DIGITS 4  // max digits on board

// --------------------------------------------------
// FUNCTION: init7seg()
// Sets up PORTA (digit select), PORTC (segments),
// and RB0/RB1 as input buttons.
// --------------------------------------------------
void init7seg() {
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;

    TRISA = 0x00;     // digit enable pins
    TRISC = 0x00;     // segment pins

    TRISBbits.TRISB0 = 1; // increment button
    TRISBbits.TRISB1 = 1; // decrement button

    PORTA = 0x00;
    PORTC = 0x00;
}

// --------------------------------------------------
// FUNCTION: clearDigits()
// Turns off all digits and clears segments.
// --------------------------------------------------
void clearDigits() {
    PORTA = 0x00;
    PORTC = 0x00;
}

// --------------------------------------------------
// FUNCTION: showDigitFor(digit, ms)
// Shows a single digit for the given number of ms.
// --------------------------------------------------
void showDigitFor(unsigned char digit, unsigned int ms) {
    if (digit > 9) return;

    PORTC = SEGMENT_TABLE[digit];
    PORTA = 0x01;        // enable digit 1 (adjust if needed)

    while (ms--) __delay_ms(1);

    clearDigits();
}

// --------------------------------------------------
// FUNCTION: showDigits(digits[], count, ms)
// Displays multiple digits for a short refresh time.
// Refuses if count > MAX_DIGITS.
// --------------------------------------------------
void showDigits(unsigned char digits[], unsigned char count, unsigned int ms) {
    if (count > MAX_DIGITS) return; // refuse if too many digits

    for (unsigned char i = 0; i < count; i++) {
        if (digits[i] > 9) continue;   // skip invalid digits
        PORTC = SEGMENT_TABLE[digits[i]];
        PORTA = (1 << i);              // enable current digit
        __delay_ms(ms);
        clearDigits();
    }
}

// --------------------------------------------------
// FUNCTION: cycleDigitsBetween(start, end, delay_ms)
// Cycles continuously from start → end → start.
// --------------------------------------------------
void cycleDigitsBetween(unsigned char start,
    unsigned char end,
    unsigned int delay_ms)
{
    if (start > 9 || end > 9) return;

    unsigned char d = start;

    while (1) {
        showDigitFor(d, delay_ms);
        d++;
        if (d > end) d = start;
    }
}

// --------------------------------------------------
// FUNCTION: incrementDigitOnRB0(current)
// Returns current+1 when RB0 is pressed.
// --------------------------------------------------
unsigned char incrementDigitOnRB0(unsigned char current) {
    if (!PORTBbits.RB0) {       // button pressed (active low)
        __delay_ms(20);         // debounce
        while (!PORTBbits.RB0); // wait for release
        current = (current + 1) % 10;
    }
    return current;
}

// --------------------------------------------------
// FUNCTION: decrementDigitOnRB1(current)
// Returns current-1 when RB1 is pressed.
// --------------------------------------------------
unsigned char decrementDigitOnRB1(unsigned char current) {
    if (!PORTBbits.RB1) {       // button pressed
        __delay_ms(20);
        while (!PORTBbits.RB1);
        current = (current == 0) ? 9 : current - 1;
    }
    return current;
}

// --------------------------------------------------
// MAIN PROGRAM
// Displays a digit and allows increment/decrement.
// --------------------------------------------------
void main() {
    init7seg();

    unsigned char digit = 0;
    unsigned char digits[4] = { 1, 2, 3, 4 }; // example multi-digit display

    while (1) {
        digit = incrementDigitOnRB0(digit);
        digit = decrementDigitOnRB1(digit);
        showDigitFor(digit, 5);          // single digit refresh
        showDigits(digits, 4, 5);        // multi-digit refresh
    }
}
