#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* ---------------- Configuration ---------------- */
#define BUZZER_DDR   DDRB
#define BUZZER_PORT  PORTB
#define BUZZER_PIN   PB1

#define SERVO_DDR    DDRB
#define SERVO_PORT   PORTB
#define SERVO_PIN    PB0

/* Servo pulse range (in microseconds) */
#define SERVO_MIN_US    500u
#define SERVO_MAX_US    2500u
#define SERVO_PERIOD_MS 20u

/* ---------------- Prototypes ------------------- */
void system_init(void);
void buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);
void buzzer_alert(uint8_t times, uint16_t on_ms, uint16_t off_ms);
void servo_init(void);
void servo_pulse_us(uint16_t pulse_us);
void servo_set_angle(uint8_t angle);
uint16_t angle_to_pulse_us(uint8_t angle);
void feeder_dispense(void);
