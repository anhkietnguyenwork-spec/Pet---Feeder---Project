#ifndef DS1307_H_
#define DS1307_H_

#include <avr/io.h>
#include "i2c.h"

#define RTC_ADDR 0x68

// ==== Hỗ trợ BCD ====
static inline uint8_t bcd_to_dec(uint8_t v) { return ((v>>4)*10 + (v & 0x0F)); }
static inline uint8_t dec_to_bcd(uint8_t v) { return ((v/10)<<4) | (v%10); }

// ==== Đọc thời gian từ DS1307 ====
void RTC_ReadTime(uint8_t *hour, uint8_t *min, uint8_t *sec) {
	I2C_Start();
	I2C_Write((RTC_ADDR << 1) | 0);   // Ghi địa chỉ ghi
	I2C_Write(0x00);                  // Thanh ghi bắt đầu: giây
	I2C_Start();
	I2C_Write((RTC_ADDR << 1) | 1);   // Địa chỉ đọc

	uint8_t s = I2C_Read(1);
	uint8_t m = I2C_Read(1);
	uint8_t h = I2C_Read(0);

	I2C_Stop();

	*sec  = bcd_to_dec(s & 0x7F);
	*min  = bcd_to_dec(m);
	*hour = bcd_to_dec(h & 0x3F);
}

// ==== Đặt giờ cho DS1307 (chạy 1 lần) ====
void RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec) {
	I2C_Start();
	I2C_Write((RTC_ADDR << 1) | 0);
	I2C_Write(0x00);
	I2C_Write(dec_to_bcd(sec));
	I2C_Write(dec_to_bcd(min));
	I2C_Write(dec_to_bcd(hour));
	I2C_Stop();
}

#endif /* DS1307_H_ */
