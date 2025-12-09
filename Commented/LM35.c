#include "lm35.h"
#include "config_bits.h"

// ----------------------------------------------------
// Initialise ADC for LM35 temperature sensor
// ----------------------------------------------------
// The LM35 is connected to AN6, which corresponds to
// analogue channel on pin RE1. Only this pin is set
// as analogue input; other PORTE pins remain digital.
void LM35_Init(void) {

    // Configure PORTE pins
    ANSELEbits.ANSE2 = 0; // RE2 as digital I/O
    ANSELEbits.ANSE1 = 1; // RE1 as analogue input (AN6 - LM35)
    ANSELEbits.ANSE0 = 0; // RE0 as digital I/O

    // Select ADC input channel
    ADCON0bits.CHS = 7;   // Channel select: AN6

    // Configure ADC voltage references
    ADCON1bits.PVCFG = 0; // Positive reference = VDD
    ADCON1bits.NVCFG = 0; // Negative reference = VSS

    // Configure ADC timing
    ADCON2bits.ACQT = 4;  // Acquisition time
    ADCON2bits.ADCS = 2;  // ADC conversion clock

    // Enable ADC module
    ADCON0bits.ADON = 1;
}

// ----------------------------------------------------
// Read temperature value from LM35 in °C
// ----------------------------------------------------
// Starts an ADC conversion, waits for completion,
// then converts the 10-bit result into degrees Celsius
// using integer arithmetic (no floating-point).
unsigned int LM35_Read_Temp(void) {

    ADCON0bits.GO = 1;        // Start ADC conversion
    __delay_ms(1);            // Short sample delay
    while (ADCON0bits.GO);    // Wait until conversion completes

    // Combine high and low ADC result registers
    unsigned int adc_val = (ADRESH << 8) + ADRESL;

    // LM35 outputs 10 mV per °C
    // ADC step ? 4.88 mV at 5 V reference
    // Scaling converts ADC value directly to °C
    unsigned long temp = (unsigned long)adc_val * 500;

    return (unsigned int)(temp / 1024);
}
