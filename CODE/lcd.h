#ifndef LCD_H_
#define LCD_H_

// SỬA LẠI THÀNH 8MHz ĐỂ DELAY CHÍNH XÁC
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

// ==== Cấu hình chân LCD (Bạn kiểm tra lại đúng chân chưa) ====
#define LCD_RS_PORT PORTB
#define LCD_RS_DDR  DDRB
#define RS          PB3

#define LCD_RW_PORT PORTB
#define LCD_RW_DDR  DDRB
#define RW          PB4

#define LCD_EN_PORT PORTB
#define LCD_EN_DDR  DDRB
#define EN          PB5

// D4–D7 nối vào PORTA
#define LCD_DATA_PORT PORTA
#define LCD_DATA_DDR  DDRA
#define D4            PA0
#define D5            PA1
#define D6            PA2
#define D7            PA3

void lcd_command(unsigned char cmnd);
void lcd_char(unsigned char data);
void lcd_init(void);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_string(const char *str);
void lcd_clear(void);
void lcd_print2(uint8_t v);
void lcd_float(float num, uint8_t digits); 
void lcd_cursor_on(void);
void lcd_cursor_off(void);

#endif /* LCD_H_ */