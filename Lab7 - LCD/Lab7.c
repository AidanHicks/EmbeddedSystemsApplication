// lcd_driver_with_buttons.c
#include <xc.h>
#include <string.h>
#include <stdio.h>

//------------------------ LCD Functions ------------------------//

void LCD_Init(void) {
    ANSELD = 0; // Make PORTD digital (data lines)
    TRISD = 0;  // RD0-RD7 as output
    ANSELE = 0; TRISE0 = 0; TRISE1 = 0; // Control lines: RS=RE0, EN=RE1
    // LCD initialization sequence goes here
}

void LCD_Command(unsigned char cmd) {
    // Send command to LCD (RS=0)
}

void LCD_WriteChar(char c) {
    // Send character to LCD (RS=1)
}

void LCD_WriteLine1(const char* str) {
    LCD_SetCursor(0, 0);
    for(unsigned char i=0; i<16 && str[i]!=0; i++)
        LCD_WriteChar(str[i]);
}

void LCD_WriteMultiLine(const char* str) {
    LCD_SetCursor(0, 0);
    unsigned char line = 0, col = 0;
    for(unsigned char i=0; str[i]!=0 && line<2; i++) {
        if(col>=16) { line++; col=0; LCD_SetCursor(line, col); }
        if(line>=2) break;
        LCD_WriteChar(str[i]);
        col++;
    }
}

void LCD_SetCursor(unsigned char line, unsigned char col) {
    unsigned char addr = (line==0) ? 0x00 : 0x40;
    addr += col;
    LCD_Command(0x80 | addr);
}

//------------------------ Button and LED ------------------------//

// Single button RB0 -> LED RB1
void ButtonLED_Init(void) {
    ANSELB = 0x00;   // PORTB digital
    TRISB0 = 1;      // RB0 input
    TRISB1 = 0;      // RB1 output
    LATB1 = 0;       // LED off
}

void CheckButtonAndLED(void) {
    RB1_bit = (RB0_bit == 0) ? 1 : 0;
}

//------------------------ Multi-button LCD Display ------------------------//

// Initialize multiple button pins on PORTB as inputs
void MultiButton_Init(void) {
    ANSELB = 0x00;       // Digital mode
    TRISB = 0xFF;        // All PORTB pins as inputs
}

// Read all PORTB buttons and display status on LCD
void DisplayButtonStates(void) {
    char buf[17];
    for(unsigned char i=0;i<8;i++){
        buf[i] = (RB0_bit >> i & 0x01) ? '1' : '0'; // read bit i
    }
    buf[8] = 0; // null terminate string
    LCD_WriteLine1(buf);
}

//------------------------ Example Main ------------------------//

void main(void) {
    LCD_Init();
    ButtonLED_Init();
    MultiButton_Init();

    LCD_WriteMultiLine("Buttons Demo\nPORTB Status");

    while(1) {
        CheckButtonAndLED();      // RB0 -> RB1 LED
        DisplayButtonStates();    // Show all button states on LCD
        __delay_ms(100);          // small delay for readability
    }
}
