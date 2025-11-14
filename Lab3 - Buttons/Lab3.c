#include <xc.h>
#define _XTAL_FREQ 16000000UL

// -----------------------------------------
// Definitions for cleaner code
// -----------------------------------------
#define BUTTON_INC PORTBbits.RB0
#define BUTTON_DEC PORTBbits.RB1
#define LED_PORT   LATC


// -----------------------------------------
// Task 1B – Blink RC0 while RB0 is held down
// -----------------------------------------
void task1B_blinkWhileHeld(void) {
    ANSELB = 0;      // PORTB digital
    ANSELC = 0;      // PORTC digital
    TRISBbits.TRISB0 = 1; // RB0 = button input
    TRISCbits.TRISC0 = 0; // RC0 = LED output

    while (1) {
        if (PORTBbits.RB0 == 0) {       // Button pressed
            LATCbits.LATC0 = 1;        // LED ON
            __delay_ms(200);
            LATCbits.LATC0 = 0;        // LED OFF
            __delay_ms(200);
        } else {
            LATCbits.LATC0 = 0;        // LED OFF when button released
        }
    }
}


// -----------------------------------------
// Task 2 – Two-button counter (increment & decrement)
// -----------------------------------------
void task2_counterTwoButtons(void) {
    ANSELB = 0;
    ANSELC = 0;

    TRISBbits.TRISB0 = 1; // RB0 = increment button
    TRISBbits.TRISB1 = 1; // RB1 = decrement button
    TRISC = 0x00;         // PORTC = LED bar output

    unsigned char counter = 0;
    unsigned char prev_inc = 1; // previous RB0 state
    unsigned char prev_dec = 1; // previous RB1 state

    LED_PORT = counter;

    while (1) {
        unsigned char cur_inc = PORTBbits.RB0;
        unsigned char cur_dec = PORTBbits.RB1;

        if (prev_inc == 1 && cur_inc == 0) {   // Falling edge RB0
            counter++;
            LED_PORT = counter;
            __delay_ms(80);                   // Basic debounce
        }

        if (prev_dec == 1 && cur_dec == 0) {   // Falling edge RB1
            counter--;
            LED_PORT = counter;
            __delay_ms(80);
        }

        prev_inc = cur_inc;
        prev_dec = cur_dec;
    }
}


// -----------------------------------------
// Task 3A – Debounced falling-edge detect on RB0
// Returns 1 once per *clean* press
// -----------------------------------------
unsigned char pressed_falling_debounced(void) {
    static unsigned char prev = 1;        // previous button state
    unsigned char cur = PORTBbits.RB0;    // current button level

    if (prev == 1 && cur == 0) {          // Possible falling edge
        __delay_ms(30);                   // Debounce delay
        if (PORTBbits.RB0 == 0) {         // Still low → valid press
            prev = 0;
            return 1;
        }
    }

    prev = cur;
    return 0;
}


// -----------------------------------------
// Task 3A – Toggle LED on each debounced press
// -----------------------------------------
void task3A_toggleWithDebounce(void) {
    ANSELB = 0;
    ANSELC = 0;

    TRISBbits.TRISB0 = 1; // Button input
    TRISCbits.TRISC0 = 0; // LED output

    LATCbits.LATC0 = 0;

    while (1) {
        if (pressed_falling_debounced()) { 
            LATCbits.LATC0 ^= 1;          // Toggle LED
        }
    }
}


// -----------------------------------------
// Task 3B – Debounced increment and decrement
// -----------------------------------------
static unsigned char prev0 = 1;
static unsigned char prev1 = 1;

unsigned char debounced_falling_RB0(void) {
    unsigned char cur = PORTBbits.RB0;
    if (prev0 == 1 && cur == 0) {         // RB0 falling edge
        __delay_ms(30);                   // debounce
        if (PORTBbits.RB0 == 0) { prev0 = 0; return 1; }
    }
    prev0 = cur;
    return 0;
}

unsigned char debounced_falling_RB1(void) {
    unsigned char cur = PORTBbits.RB1;
    if (prev1 == 1 && cur == 0) {         // RB1 falling edge
        __delay_ms(30);
        if (PORTBbits.RB1 == 0) { prev1 = 0; return 1; }
    }
    prev1 = cur;
    return 0;
}

void task3B_counterDebounced(void) {
    ANSELB = 0;
    ANSELC = 0;

    TRISBbits.TRISB0 = 1; // RB0 = increment
    TRISBbits.TRISB1 = 1; // RB1 = decrement
    TRISC = 0x00;         // LED bar outputs

    unsigned char counter = 0;
    LED_PORT = counter;

    while (1) {
        if (debounced_falling_RB0()) {    // Clean increment
            counter++;
            LED_PORT = counter;
        }

        if (debounced_falling_RB1()) {    // Clean decrement
            counter--;
            LED_PORT = counter;
        }
    }
}


// -----------------------------------------
// MAIN PROGRAM – Choose which task to run
// -----------------------------------------
void main(void) {

    // Uncomment ONE of the tasks to run it:

    //task1B_blinkWhileHeld();
    //task2_counterTwoButtons();
    //task3A_toggleWithDebounce();
    task3B_counterDebounced();

    while (1);
}
