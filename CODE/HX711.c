#include "hx711.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // C?n cho hàm cli() và sei()

/**
 * @brief Kh?i t?o các chân I/O cho HX711
 * - SCK: Output (M?c th?p)
 * - DOUT: Input
 */
void hx711_init(void) {
    // C?u h?nh chân SCK là Output
    HX711_SCK_DDR |= (1 << HX711_SCK_PIN);
    
    // Ð?t m?c th?p cho SCK ban ð?u
    HX711_SCK_PORT &= ~(1 << HX711_SCK_PIN);
    
    // C?u h?nh chân DOUT là Input
    HX711_DOUT_DDR &= ~(1 << HX711_DOUT_BIT);
    
    // (Tùy ch?n) B?t ði?n tr? kéo lên cho DOUT n?u c?n
    // HX711_DOUT_PORT |= (1 << HX711_DOUT_BIT); 
}

/**
 * @brief Ð?c giá tr? thô 24-bit t? HX711
 * @return Giá tr? s? nguyên 32-bit có d?u
 */
int32_t hx711_read_raw(void) {
    // 1. Ch? cho chân DOUT xu?ng m?c th?p (Chip s?n sàng)
    // N?u DOUT ðang cao ngh?a là chip ðang b?n chuy?n ð?i
    while (HX711_DOUT_PORT & (1 << HX711_DOUT_BIT));

    uint32_t count = 0;
    uint8_t i;

    // T?t ng?t toàn c?c ð? ð?m b?o th?i gian t?o xung clock chính xác
    cli();

    // 2. Ð?c 24 bit d? li?u (MSB trý?c)
    for (i = 0; i < 24; i++) {
        // T?o xung Clock lên m?c CAO
        HX711_SCK_PORT |= (1 << HX711_SCK_PIN);
        _delay_us(1); // Gi? m?c cao 1us
        
        // D?ch giá tr? c? sang trái 1 bit ð? ðón bit m?i
        count = count << 1;
        
        // H? xung Clock xu?ng m?c TH?P
        HX711_SCK_PORT &= ~(1 << HX711_SCK_PIN);
        _delay_us(1); // Gi? m?c th?p 1us (ð? th?i gian ð? chip ð?y d? li?u ra)
        
        // Ð?c bit t? chân DOUT
        if (HX711_DOUT_PORT & (1 << HX711_DOUT_BIT)) {
            count++; // N?u chân DOUT là 1, c?ng thêm 1 vào bit cu?i
        }
    }

    // 3. G?i xung Clock th? 25, 26, 27 ð? c?u h?nh Gain cho l?n ð?c TI?P THEO
    // - 25 xung: Kênh A, Gain 128 (M?c ð?nh)
    // - 26 xung: Kênh B, Gain 32
    // - 27 xung: Kênh A, Gain 64
    
    // ? ðây ta dùng 1 xung thêm (t?ng 25) -> Gain 128
    HX711_SCK_PORT |= (1 << HX711_SCK_PIN);
    _delay_us(1);
    HX711_SCK_PORT &= ~(1 << HX711_SCK_PIN);
    _delay_us(1);
    
    // B?t l?i ng?t
    sei();

    // 4. X? l? s? âm (Sign Extension)
    // Giá tr? t? HX711 là s? có d?u 24-bit (bù 2).
    // N?u bit th? 23 (bit d?u) là 1, ngh?a là s? âm.
    // Ta c?n m? r?ng d?u ra 32-bit ð? bi?n int32_t hi?u ðúng.
    if (count & 0x00800000) {
        count |= 0xFF000000;
    }

    return (int32_t)count;
}

/**
 * @brief Ð?c trung b?nh nhi?u l?n ð? l?c nhi?u
 * @param times S? l?n ð?c
 * @return Giá tr? trung b?nh
 */
int32_t hx711_get_average(uint8_t times) {
    int64_t sum = 0; // Dùng 64-bit ð? tránh tràn s? khi c?ng d?n
    
    // Ð?m b?o ð?c ít nh?t 1 l?n
    if (times < 1) times = 1;

    for (uint8_t i = 0; i < times; i++) {
        sum += hx711_read_raw();
        // Không c?n delay gi?a các l?n ð?c v? hàm read_raw ð? có v?ng l?p ch?
    }
    
    return (int32_t)(sum / times);}