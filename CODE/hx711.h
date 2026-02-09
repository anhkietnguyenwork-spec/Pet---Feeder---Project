#ifndef HX711_H_
#define HX711_H_

#include <avr/io.h>
#include <stdint.h>

// --- Ð?nh ngh?a ph?n c?ng ---
#define HX711_SCK_PORT  PORTB
#define HX711_SCK_DDR   DDRB
#define HX711_SCK_PIN   PB7
#define HX711_DOUT_PORT PINB
#define HX711_DOUT_DDR  DDRB
#define HX711_DOUT_BIT  PB6

#define SCALE_FACTOR 435.00f 

// --- Khai báo hàm (Prototype) ---
void hx711_init(void);
int32_t hx711_read_raw(void); // Ð?i tên cho r? ràng
int32_t hx711_get_average(uint8_t times);

#endif /* HX711_H_ */