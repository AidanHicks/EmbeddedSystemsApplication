// buzzer.c
// PIC18(L)F2X/4XK22 - Buzzer interfacing on RC2
// Generates continuous tone, melody, multi-tone chime, and button-triggered note

#include <xc.h>

#define BUZZER LATC2_bit  // Buzzer pin
#define BUTTON RB0_bit    // Button input

// ===================== Task 1: Continuous Tone =====================
void playTone(unsigned int freq, unsigned int duration_ms){
    unsigned long period_us = 1000000UL / freq;
    unsigned long half_period = period_us / 2UL;
    unsigned long cycles = ((unsigned long)duration_ms * 1000UL) / period_us;

    for(unsigned long i = 0; i < cycles; i++){
        BUZZER = 1;
        Delay_us(half_period);
        BUZZER = 0;
        Delay_us(half_period);
    }
}

void continuousTone(unsigned int freq){
    while(1){
        playTone(freq, 10); // short chunks to keep MCU responsive
    }
}

// ===================== Task 2: Simple Melody =====================
unsigned int melodyNotes[] = { 264, 264, 297, 264, 352, 330, 264, 264, 297, 264, 396, 352 };
unsigned char melodyDurations[] = { 250, 250, 500, 500, 500, 1000, 250, 250, 500, 500, 500, 1000 };

void playMelody(void){
    for(unsigned char i = 0; i < sizeof(melodyNotes)/sizeof(melodyNotes[0]); i++){
        playTone(melodyNotes[i], melodyDurations[i]);
        Delay_ms(50);
    }
}

// ===================== Task 3: Multi-Tone Chime =====================
unsigned int chimeNotes[] = { 440, 554, 659 };
unsigned char chimeDur[] = { 200, 200, 200 };

void playChime(void){
    for(unsigned char i = 0; i < sizeof(chimeNotes)/sizeof(chimeNotes[0]); i++){
        playTone(chimeNotes[i], chimeDur[i]);
        Delay_ms(50);
    }
}

// ===================== Task 4: Play note on button press =====================
// Press BUTTON to play a note of frequency `freq` for `duration_ms` milliseconds
void playNoteOnButtonPress(unsigned int freq, unsigned int duration_ms){
    static unsigned char prevButton = 1;

    if(prevButton == 1 && BUTTON == 0){ // detect falling edge
        Delay_ms(30); // debounce
        if(BUTTON == 0){
            playTone(freq, duration_ms);
        }
    }
    prevButton = BUTTON;
}

// ===================== Initialization =====================
void initBuzzer(void){
    TRISC2_bit = 0;  // RC2 output
    BUZZER = 0;
    ANSELC2_bit = 0;  // digital
    TRISB0_bit = 1;   // button input
    ANSELB0_bit = 0;  // digital input
}

// ===================== Example main for button-triggered note =====================
void buzzerMain(void){
    initBuzzer();
    while(1){
        playNoteOnButtonPress(440, 500); // A4 note, 500 ms
    }
}
