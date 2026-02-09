#include "lcd.h"

static void lcd_enable_pulse(void)
{
	LCD_EN_PORT |= (1<<EN);
	_delay_us(1); // Delay ngắn cho xung Enable
	LCD_EN_PORT &= ~(1<<EN);
	_delay_us(100); // Chờ LCD xử lý
}

static void lcd_send4bit(uint8_t data)
{
	// Xóa 4 bit thấp của PORT data trước
	LCD_DATA_PORT &= ~((1<<D4)|(1<<D5)|(1<<D6)|(1<<D7));
	
	// Ghi dữ liệu mới
	if (data & (1<<4)) LCD_DATA_PORT |= (1<<D4);
	if (data & (1<<5)) LCD_DATA_PORT |= (1<<D5);
	if (data & (1<<6)) LCD_DATA_PORT |= (1<<D6);
	if (data & (1<<7)) LCD_DATA_PORT |= (1<<D7);
}

void lcd_command(unsigned char cmnd)
{
	LCD_RS_PORT &= ~(1<<RS);
	LCD_RW_PORT &= ~(1<<RW);

	lcd_send4bit(cmnd & 0xF0);
	lcd_enable_pulse();

	lcd_send4bit((cmnd << 4) & 0xF0);
	lcd_enable_pulse();

	_delay_ms(2); // Delay 2ms cho lệnh (quan trọng ở 8MHz)
}

void lcd_char(unsigned char data)
{
	LCD_RS_PORT |= (1<<RS);
	LCD_RW_PORT &= ~(1<<RW);

	lcd_send4bit(data & 0xF0);
	lcd_enable_pulse();

	lcd_send4bit((data << 4) & 0xF0);
	lcd_enable_pulse();

	_delay_ms(1);
}

void lcd_init(void)
{
	LCD_RS_DDR |= (1<<RS);
	LCD_RW_DDR |= (1<<RW);
	LCD_EN_DDR |= (1<<EN);
	LCD_DATA_DDR |= (1<<D4)|(1<<D5)|(1<<D6)|(1<<D7);

	_delay_ms(50); // Chờ nguồn ổn định

	lcd_command(0x02); // Về home
	lcd_command(0x28); // 4-bit mode
	lcd_command(0x0C); // Bật hiển thị, tắt con trỏ
	lcd_command(0x06); // Tự động tăng
	lcd_command(0x01); // Xóa màn hình
	_delay_ms(2);
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{
	unsigned char address = (y == 0) ? (0x80 + x) : (0xC0 + x);
	lcd_command(address);
}

void lcd_string(const char *str)
{
	while (*str) lcd_char(*str++);
}

void lcd_clear(void)
{
	lcd_command(0x01);
	_delay_ms(2);
}

void lcd_print2(uint8_t v)
{
	lcd_char('0' + (v / 10));
	lcd_char('0' + (v % 10));
}
void lcd_cursor_on(void) {
	// 0x0F = Display ON, Cursor ON, Blink ON
	lcd_command(0x0F);
}

/* Tắt con trỏ (Dùng ở màn hình bình thường) */
void lcd_cursor_off(void) {
	// 0x0C = Display ON, Cursor OFF, Blink OFF
	lcd_command(0x0C);
}
void lcd_float(float num, uint8_t digits)
{
	char buffer[16];
	int32_t int_part;
	uint32_t frac_part;
	
	if (digits > 6) digits = 6;

	// Xử lý số âm
	if (num < 0)
	{
		lcd_char('-');
		num = -num;
	}

	int_part = (int32_t)num;
	
	// Tính phần thập phân
	// Ví dụ: 12.34, digits=2 -> (12.34 - 12) * 100 = 0.34 * 100 = 34
	float remainder = num - int_part;
	frac_part = (uint32_t)(remainder * pow(10, digits) + 0.5);

	// In phần nguyên
	itoa(int_part, buffer, 10);
	lcd_string(buffer);

	if (digits == 0) return;

	lcd_char('.'); // In dấu chấm

	// In phần thập phân và bù số 0 phía trước nếu cần
	// Ví dụ: 12.05, frac_part = 5. Cần in "05" chứ không phải "5"
	char frac_buff[10];
	itoa(frac_part, frac_buff, 10);
	
	uint8_t len = 0;
	while(frac_buff[len] != '\0') len++;
	
	while(len < digits) {
		lcd_char('0');
		len++;
	}
	
	lcd_string(frac_buff);
}