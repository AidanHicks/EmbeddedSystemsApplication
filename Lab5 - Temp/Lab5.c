// temperature.c
// PIC18(L)F2X/4XK22 - LM35 temperature display on 4-digit 7-segment
// RD0=a .. RD7=dp, RA0..RA3 digit enables, common-cathode

#include <xc.h>

#define SCAN_ON_US 900
#define BLANK_US 80
#define SAMPLE_FRAMES 250
#define MA_N 8
#define VREF_mV 5000u  // Change to 3300u if J5 = 3.3 V

// Lookup table for common-cathode 7-segment
unsigned char seg_for(unsigned char d){
    static const unsigned char CC[10] = {
        0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F
    };
    return CC[d % 10u];
}

// Turn off all digits
void all_off(void){ LATA = 0x00; }

// Enable a specific digit (0..3)
void enable_pos(unsigned char pos){ LATA = (unsigned char)(1u << pos); }

// Split integer 0..9999 into four decimal digits
void split4(unsigned int v, unsigned char *u, unsigned char *t, unsigned char *h, unsigned char *th){
    *u = v % 10u;
    *t = (v / 10u) % 10u;
    *h = (v / 100u) % 10u;
    *th = (v / 1000u) % 10u;
}

// Initialize ADC for LM35 (AN6 = RE1)
void ADC_Init(void){
    ANSELE = 0x02;    // RE1 analog
    TRISE1_bit = 1;   // input
    ADCON1 = 0x00;    // Vref+ = VDD, Vref- = VSS
    ADCON2 = 0xA9;    // right-justified, safe acquisition
}

// Sample ADC channel 0..15, return 10-bit result
unsigned int ADC_Get_Sample(unsigned char ch){
    ADCON0 = (unsigned char)(ch << 2); // select channel
    ADCON0.F0 = 1;                     // ADON
    Delay_ms(2);
    ADCON0.F1 = 1;                     // GO/DONE
    while(ADCON0.F1);
    return (unsigned int)((((unsigned int)ADRESH) << 8) | ADRESL);
}

// Convert raw ADC to temperature in 0.01°C (T100), integer math
unsigned int adc_to_T100(unsigned int adc){
    unsigned long t = (unsigned long)adc * (unsigned long)VREF_mV * 10ul; // adc*mV*10
    t = (t + 511ul) / 1023ul; // rounding
    if(t > 9999ul) t = 9999ul; // clamp to display
    return (unsigned int)t;
}

// Initialize 7-segment display ports
void init7seg(void){
    ANSELA = 0; ANSELB = 0; ANSELD = 0;
    CM1CON0 = 0; CM2CON0 = 0;
    TRISD = 0; // RD0..RD7 output
    TRISA0_bit = 0; TRISA1_bit = 0; TRISA2_bit = 0; TRISA3_bit = 0; // digit enables
    LATD = 0; all_off();
}

// Display temperature on 4-digit 7-segment with decimal (XX.XX °C)
void displayTemperature(unsigned int T100){
    static unsigned char pos = 0;
    unsigned char u,t,h,th;

    split4(T100, &u, &t, &h, &th);

    // Multiplexed display
    all_off(); LATD = 0; Delay_us(BLANK_US);
    if(pos == 0u) LATD = seg_for(u);                // units of 0.01°C
    else if(pos == 1u) LATD = (unsigned char)(seg_for(t) | 0x80u); // tens with decimal
    else if(pos == 2u) LATD = seg_for(h);
    else LATD = (th == 0u ? 0x00 : seg_for(th));   // optional leading zero blank
    enable_pos(pos); Delay_us(SCAN_ON_US);
    all_off(); LATD = 0; Delay_us(BLANK_US);

    pos++; if(pos >= 4u) pos = 0u;
}

// Main temperature loop with moving-average smoothing
void temperatureLoop(void){
    unsigned int T100, T100_avg, buf[MA_N];
    unsigned long sum = 0;
    unsigned char idx = 0, i = 0, frame = 0;

    init7seg();
    ADC_Init();

    // Seed moving average
    for(i=0; i<MA_N; i++){
        buf[i] = adc_to_T100(ADC_Get_Sample(6));
        sum += buf[i];
    }
    T100_avg = (unsigned int)(sum / MA_N);

    while(1){
        // Take new sample every SAMPLE_FRAMES display frames
        if(++frame >= SAMPLE_FRAMES){
            frame = 0;
            sum -= buf[idx];
            T100 = adc_to_T100(ADC_Get_Sample(6));
            buf[idx] = T100;
            sum += T100;
            if(++idx >= MA_N) idx = 0;
            T100_avg = (unsigned int)(sum / MA_N);
        }

        // Show averaged temperature
        displayTemperature(T100_avg);
    }
}
