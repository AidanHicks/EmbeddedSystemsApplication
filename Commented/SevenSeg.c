#include "sevenseg.h"


// Segment patterns for digits 0?9 (Common Cathode)

// Each byte represents segments a?g + dp.
// Logic '1' turns a segment ON.
const unsigned char SEG_MAP[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};


// Display buffer and refresh state

// digits[] holds the segment patterns for each digit.
// current_digit tracks which digit is being refreshed.
volatile unsigned char digits[4] = {0, 0, 0, 0};
volatile unsigned char current_digit = 0;


// Initialise seven-segment display and Timer0


// Multiplexing- A Timer0 interrupt refreshes the digits fast enough

void SevenSeg_Init(void) {

    TRISD = 0x00;      // PORTD drives segment lines (a?g, dp)
    TRISA &= 0xF0;     // RA0?RA3 control digit enables (outputs)
    
    LATA &= 0xF0;      // All digits OFF initially
    LATD = 0x00;       // All segments OFF
    
    T0CONbits.T08BIT = 0; // 16-bit timer mode
    T0CONbits.T0CS = 0;   // Internal instruction clock
    T0CONbits.PSA = 0;    // Enable prescaler
    T0CONbits.T0PS = 3;   // 1:16 prescaler
    
    TMR0H = 0xFF;         // Timer preload high byte
    TMR0L = 0x06;         // 2ms refresh rate
    
    INTCONbits.TMR0IE = 1;// Enable Timer0 interrupts
    T0CONbits.TMR0ON = 1; // Start Timer0
}


// Update the number to be displayed

// Splits an integer value into its individual digits
// and loads the corresponding segment patterns into
// the display buffer.
void SevenSeg_Update_Value(unsigned int number) {

    if(number > 9999) number = 9999; // Clamp to display range
    
    digits[0] = SEG_MAP[number / 1000];       // Thousands
    digits[1] = SEG_MAP[(number / 100) % 10]; // Hundreds
    digits[2] = SEG_MAP[(number / 10) % 10];  // Tens
    digits[3] = SEG_MAP[number % 10];         // Units
}


// Seven-segment display ISR handler

// Called from the Timer0 interrupt.
// Refreshes ONE digit per interrupt to achieve multiplexing

void SevenSeg_ISR_Handler(void) {

    LATA &= 0xF0; // Turn OFF all digits before updating
    
    // Load segment pattern
    LATD = digits[3 - current_digit];

    // Enable the active digit
    switch(current_digit) {
        case 0: LATA |= 0x01; break; // RA0 ? rightmost digit
        case 1: LATA |= 0x02; break; // RA1
        case 2: LATA |= 0x04; break; // RA2
        case 3: LATA |= 0x08; break; // RA3 ? leftmost digit
    }

    // Move to next digit for next interrupt
    current_digit++;
    if(current_digit > 3) current_digit = 0;

    // Reload Timer0 for next 2ms interval
    TMR0H = 0xFF;
    TMR0L = 0x06;
    INTCONbits.TMR0IF = 0; // Clear interrupt flag
}
