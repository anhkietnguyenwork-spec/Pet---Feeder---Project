#include "servobuzzer.h"

void system_init(void)
{
	buzzer_init();
	servo_init();
	
	// --- QUAN TR?NG: Ð?t v? trí ban ð?u cho Servo ---
	// Ð? servo "ch?" ? v? trí 180 ð? (Hý?ng lên) ngay khi b?t máy
	servo_set_angle(180); 
}

/* ---- Buzzer ---- */
void buzzer_init(void)
{
	BUZZER_DDR |= (1 << BUZZER_PIN);
	buzzer_off();
}

void buzzer_on(void)  { BUZZER_PORT |=  (1 << BUZZER_PIN); }
void buzzer_off(void) { BUZZER_PORT &= ~(1 << BUZZER_PIN); }

void buzzer_alert(uint8_t times, uint16_t on_ms, uint16_t off_ms)
{
	for (uint8_t i = 0; i < times; ++i) {
		buzzer_on();
		for (uint16_t j = 0; j < on_ms; j++) _delay_ms(1);
		buzzer_off();
		for (uint16_t j = 0; j < off_ms; j++) _delay_ms(1);
	}
}

/* ---- Servo ---- */
void servo_init(void)
{
	SERVO_DDR |= (1 << SERVO_PIN);
	SERVO_PORT &= ~(1 << SERVO_PIN);
}

/* G?i 1 xung PWM th? công (Chính xác) */
void servo_pulse_us(uint16_t pulse_us)
{
	if (pulse_us < SERVO_MIN_US) pulse_us = SERVO_MIN_US;
	if (pulse_us > SERVO_MAX_US) pulse_us = SERVO_MAX_US;

	SERVO_PORT |= (1 << SERVO_PIN); // M?c cao

	// Delay chính xác
	uint16_t us = pulse_us;
	if(us > 20) us -= 20;

	while (us >= 100) { _delay_us(100); us -= 100; }
	while (us >= 10)  { _delay_us(10);  us -= 10;  }
	while (us > 0)    { _delay_us(1);   us--;      }

	SERVO_PORT &= ~(1 << SERVO_PIN); // M?c th?p

	_delay_ms(18); // Ngh? 18ms
}

uint16_t angle_to_pulse_us(uint8_t angle)
{
	if (angle > 180) angle = 180;
	uint32_t span = (uint32_t)SERVO_MAX_US - SERVO_MIN_US;
	uint32_t pulse = (uint32_t)SERVO_MIN_US + ((uint32_t)angle * span) / 180u;
	return (uint16_t)pulse;
}

void servo_set_angle(uint8_t angle)
{
	uint16_t pulse = angle_to_pulse_us(angle);
	// G?i 50 xung ð? servo k?p ch?y ð?n v? trí
	for (uint8_t i = 0; i < 50; ++i) {
		servo_pulse_us(pulse);
	}
	SERVO_PORT &= ~(1 << SERVO_PIN); // Ng?t xung ð? ngh?
}

/* * ---- HÀM CHO ÃN (Ð? Ð?O NGÝ?C HÝ?NG) ---- 
 * Logic m?i: 
 * 1. Ban ð?u servo ðang ? 180 (do system_init)
 * 2. Khi báo th?c: C?i kêu -> Servo quay xu?ng 0 -> Ch? -> Quay lên 180
 */
void feeder_dispense(void)
{
	// 1. C?i kêu báo hi?u
	buzzer_alert(6, 100, 100);
	
	// 2. Quay xu?ng 0 ð? (M? khay/Ð? th?c ãn)
	// (Trý?c ðây là 180, gi? ð?i thành 0)
	servo_set_angle(0);
	
	// 3. Gi? ? dý?i 2 giây
	_delay_ms(2000);
	
	// 4. Quay ngý?c lên 180 ð? (Ðóng khay/V? v? trí ngh?)
	// (Trý?c ðây là 0, gi? ð?i thành 180)
	servo_set_angle(180);
	
	buzzer_off();
}