#include "lcd.h"
#include "config_bits.h"


// Send a 4-bit nibble to the LCD (4-bit interface)
// The LCD is operated in 4-bit mode 
// Each char is sent as two nibbles (high then low).

void LCD_Nibble(char nibble) {
    // Output the nibble bits onto LCD data lines D4?D7
    LCD_D4 = (nibble >> 0) & 1;
    LCD_D5 = (nibble >> 1) & 1;
    LCD_D6 = (nibble >> 2) & 1;
    LCD_D7 = (nibble >> 3) & 1;

    // Enable pulse latches data into LCD controller
    LCD_EN = 1;
    __delay_us(50);
    LCD_EN = 0;
    __delay_us(50);
}


// Send a command byte to the LCD (RS = 0)

// Commands control LCD behaviour such as clearing the
// display or moving the cursor.
void LCD_Cmd(char cmd) {
    LCD_RS = 0;                   // Select command register
    LCD_Nibble(cmd >> 4);         // Send high nibble
    LCD_Nibble(cmd);              // Send low nibble
    __delay_ms(2);                // Command execution delay
}


// Write a single character to the LCD (RS = 1)

// Character data is written into the LCD DDRAM and
// appears at the current cursor position.
void LCD_Char(char dat) {
    LCD_RS = 1;                   // Select data register
    LCD_Nibble(dat >> 4);         // High nibble
    LCD_Nibble(dat);              // Low nibble
    __delay_us(50);               // Short data hold delay
}


// Initialise the LCD module

// Follows the HD44780 initialisation sequence to force
// the LCD into a known 4-bit operating mode.
void LCD_Init(void) {
    TRISB = 0x00;                 // PORTB used for LCD I/O
    LCD_RS = 0;
    LCD_EN = 0;

    __delay_ms(20);               // LCD power-up delay
    
    // Reset sequence required by LCD datasheet
    LCD_Nibble(0x03); __delay_ms(5);
    LCD_Nibble(0x03); __delay_us(150);
    LCD_Nibble(0x03);
    LCD_Nibble(0x02);             // Switch to 4-bit mode
    
    // LCD configuration commands
    LCD_Cmd(0x28);                // 2-line display, 5×8 font
    LCD_Cmd(0x0C);                // Display ON, cursor OFF
    LCD_Cmd(0x06);                // Entry mode: auto increment
    LCD_Clear();                  // Clear display and reset cursor
}


// Write a null-terminated string to the LCD

// Characters are written sequentially starting from
// the current cursor position.
void LCD_String(const char* str) {
    while(*str)
        LCD_Char(*str++);
}


// Set LCD cursor position

// Row 1 corresponds to DDRAM address 0x80
// Row 2 corresponds to DDRAM address 0xC0
void LCD_Set_Cursor(unsigned char row, unsigned char col) {
    unsigned char address;
    if(row == 1)
        address = 0x80 + col;     // First LCD row
    else
        address = 0xC0 + col;     // Second LCD row

    LCD_Cmd(address);
}


// Clear LCD display

// Clears all characters and resets cursor to (0,0).
void LCD_Clear(void) {
    LCD_Cmd(0x01);                // Clear display command
    __delay_ms(2);                // Clear operation delay
}
