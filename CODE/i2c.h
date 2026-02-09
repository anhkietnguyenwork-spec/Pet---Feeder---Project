ï»¿#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <util/delay.h>

void I2C_Init(void) {
	TWSR = 0x00;
	TWBR = 32;  // 100kHz vá»›i F_CPU = 1MHz
}

void I2C_Start(void) {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

void I2C_Stop(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	_delay_us(10);
}

void I2C_Write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

uint8_t I2C_Read(uint8_t ack) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(ack<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

#endif /* I2C_H_ */
