#include <xc.h>
#define _XTAL_FREQ 16000000UL

void CLEARLED() {
    LATC = 0x00;
}

void SET_LED(unsigned char pin) {
    LATC = (1 << pin);
}

void Blink() {
    TRISC = 0x00;
    while (1) {
        LATC = 0x01;
        __delay_ms(500);
        LATC = 0x00;
        __delay_ms(500);
    }
}

void BlinkALL() {
    const unsigned int ON_MS = 200;
    const unsigned int OFF_MS = 800;
    TRISC = 0x00;
    LATC = 0xFF;
    __delay_ms(ON_MS);
    LATC = 0x00;
    __delay_ms(OFF_MS);
}

void Sweep() {
    signed char i;
    TRISC = 0x00;
    LATC = 0x00;
    while (1) {
        __delay_ms(500);
        for (i = 0; i <= 7; i++) {
            LATC = (1 << i);
            __delay_ms(120);
        }
        __delay_ms(500);
        for (i = 6; i >= 0; i--) {
            LATC = (1 << i);
            __delay_ms(120);
        }
    }
}

void sweep_forward() {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        SET_LED(i);
        __delay_ms(200);
    }
}

void sweep_backward() {
    signed char i;
    for (i = 7; i >= 0; i--) {
        SET_LED(i);
        __delay_ms(200);
    }
}

void pause_at_end() {
    __delay_ms(500);
}

void SweepSpeedControl() {
    const int SPEED_MS = 20;
    unsigned char i;
    TRISC = 0x00;
    while (1) {
        for (i = 0; i < 8; i++) {
            LATC = (1 << i);
            __delay_ms(SPEED_MS);
        }
    }
}

void main() {
    TRISC = 0x00;
    CLEARLED();
    while (1) {
        sweep_forward();
        pause_at_end();
        BlinkALL();
        sweep_backward();
        pause_at_end();
        BlinkALL();
    }
}
