/*
 * File main.c
 * H? th?ng cho thú cýng ãn t? ð?ng - Full Option
 * Ch?c nãng: Hi?n th? RTC/Loadcell, C?nh báo <20g, Cài ð?t Gi?/Phút/Giây
 */

#define F_CPU 8000000UL // Xung nh?p 8MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h> 

// Include các thý vi?n
#include "lcd.h"    
#include "i2c.h"
#include "ds1307.h"
#include "hx711.h"
#include "keypad.h"
#include "servobuzzer.h" 

// --- BI?N TOÀN C?C ---
int32_t TARE_OFFSET = 0; // Giá tr? tr? b?

// Bi?n lýu gi? báo th?c (Lýu trong RAM)
// M?c ð?nh: Alarm 1 lúc 07:00:00, Alarm 2 lúc 17:00:00
uint8_t alarm1_h = 7,  alarm1_m = 0, alarm1_s = 0;
uint8_t alarm2_h = 17, alarm2_m = 0, alarm2_s = 0;

// Hàm hi?n th? giao di?n cài ð?t
// cursor_pos: 0-5 (0=Ch?c gi?, 1=Ðõn v? gi?, ..., 5=Ðõn v? giây)
void display_set_time_screen(uint8_t alarm_num, uint8_t h, uint8_t m, uint8_t s, uint8_t cursor_pos) {
    // Mapping v? trí con tr? logic sang v? trí c?t LCD
    // Format: HH:MM:SS
    // Index:  01 34 67
    uint8_t lcd_map[] = {0, 1, 3, 4, 6, 7}; 

    lcd_gotoxy(0, 0);
    if (alarm_num == 1) lcd_string("SET TIME 1:     ");
    else                lcd_string("SET TIME 2:     ");

    lcd_gotoxy(0, 1);
    lcd_print2(h); lcd_char(':');
    lcd_print2(m); lcd_char(':');
    lcd_print2(s);
    lcd_string("  (D:OK)");

    // Ð?t con tr? nh?p nháy ðúng s? ðang s?a
    lcd_gotoxy(lcd_map[cursor_pos], 1);
}

// Hàm x? l? logic cài ð?t (Ch?nh Gi?, Phút, Giây)
void set_alarm_mode(uint8_t alarm_num) {
    uint8_t h, m, s;
    uint8_t pos = 0; // 0->5
    char key;

    // L?y giá tr? hi?n t?i
    if (alarm_num == 1) { h = alarm1_h; m = alarm1_m; s = alarm1_s; }
    else                { h = alarm2_h; m = alarm2_m; s = alarm2_s; }

    lcd_clear();
    lcd_cursor_on(); // B?t con tr?

    display_set_time_screen(alarm_num, h, m, s, pos);

    while(1) {
        key = read_keypad();
        if (key) {
            // --- DI CHUY?N CON TR? (L/R) ---
            if (key == 'L') { 
                if (pos > 0) pos--; 
            }
            else if (key == 'R') { 
                if (pos < 5) pos++; 
            }
            
            // --- XÁC NH?N (D) ---
            else if (key == 'D') {
                // Lýu l?i vào bi?n toàn c?c
                if (alarm_num == 1) { alarm1_h = h; alarm1_m = m; alarm1_s = s; }
                else                { alarm2_h = h; alarm2_m = m; alarm2_s = s; }
                break; // Thoát
            }
            
            // --- NH?P S? (0-9) ---
            else if (key >= '0' && key <= '9') {
                uint8_t val = key - '0';
                switch(pos) {
                    case 0: h = (h % 10) + val * 10; break; // Gi? ch?c
                    case 1: h = (h / 10) * 10 + val; break; // Gi? ðõn v?
                    case 2: m = (m % 10) + val * 10; break; // Phút ch?c
                    case 3: m = (m / 10) * 10 + val; break; // Phút ðõn v?
                    case 4: s = (s % 10) + val * 10; break; // Giây ch?c
                    case 5: s = (s / 10) * 10 + val; break; // Giây ðõn v?
                }
                
                // Gi?i h?n h?p l?
                if (h > 23) h = 23;
                if (m > 59) m = 59;
                if (s > 59) s = 59;
                
                // T? ð?ng nh?y sang ph?i cho ti?n
                if (pos < 5) pos++;
            }
            
            display_set_time_screen(alarm_num, h, m, s, pos);
            _delay_ms(200); // Ch?ng d?i phím
        }
    }

    lcd_cursor_off(); // T?t con tr?
    lcd_clear();
    lcd_string("DA LUU CAI DAT!");
    _delay_ms(1000);
    lcd_clear();
}

int main(void)
{
    uint8_t hour, min, sec, old_sec = 0xFF;
    float weight_gram = 0.0;
    int32_t gia_tri_raw, gia_tri_net;
    char key;

    // 1. KH?I T?O
    lcd_init();     
    I2C_Init();     
    hx711_init();
    keypad_init();   
    system_init();   // Kh?i t?o Buzzer, Motor
    
    // 2. TR? B? LOADCELL
    lcd_clear();
    lcd_string("Dang khoi dong..");
    TARE_OFFSET = hx711_get_average(10); 
    _delay_ms(500);
    lcd_clear();

    // --- CÀI GI? DS1307 (N?U C?N TH? B? COMMENT) ---
    RTC_SetTime(14, 34, 00); 

    // 3. V?NG L?P CHÍNH
    while (1)
    {
        // A. Ð?C D? LI?U
        RTC_ReadTime(&hour, &min, &sec);

        gia_tri_raw = hx711_get_average(5); // Ð?c trung b?nh 5 l?n
        gia_tri_net = gia_tri_raw - TARE_OFFSET;
        weight_gram = (float)gia_tri_net / SCALE_FACTOR; 
        if (weight_gram < 0.0f) weight_gram = 0.0f;      

        // B. X? L? KEYPAD
        key = read_keypad();
        
        if (key == 'A') {
            set_alarm_mode(1); // Cài gi? cho ãn 1
        }
        else if (key == 'B') {
            set_alarm_mode(2); // Cài gi? cho ãn 2
        }
        else if (key == 'C') {
            // Cho ãn ngay l?p t?c
            lcd_clear();
            lcd_string("Dang cho an...");
            feeder_dispense(); // C?i kêu -> Motor quay
            lcd_clear();
        }

        // C. KI?M TRA BÁO TH?C (Ðúng gi? phút giây)
        // Alarm 1
        if (hour == alarm1_h && min == alarm1_m && sec == alarm1_s) {
            lcd_clear();
            lcd_string("DEN GIO AN 1!");
            feeder_dispense(); // Th?c hi?n cho ãn
            lcd_clear();
        }
        // Alarm 2
        else if (hour == alarm2_h && min == alarm2_m && sec == alarm2_s) {
            lcd_clear();
            lcd_string("DEN GIO AN 2!");
            feeder_dispense(); // Th?c hi?n cho ãn
            lcd_clear();
        }

        // D. C?NH BÁO H?T TH?C ÃN (< 20g)
        // (Ch? ki?m tra khi không ph?i gi? cho ãn ð? tránh xung ð?t)
        if (weight_gram <= 20.0f) {
            buzzer_alert(1, 100, 0); // Kêu bíp ng?n
        } 

        // E. HI?N TH? LCD
        if (sec != old_sec) {
            old_sec = sec;
            
            // Hàng 1: Th?i gian "12:30:45"
            lcd_gotoxy(0, 0);
            lcd_print2(hour); lcd_char(':');
            lcd_print2(min);  lcd_char(':');
            lcd_print2(sec);
            


            // Hàng 2: Kh?i lý?ng "Mass: 123.4g"
            lcd_gotoxy(0, 1);
            lcd_string("Mass: ");
            lcd_float(weight_gram, 1);
            lcd_string("g   "); 
        }
        
        _delay_ms(100); 
    }
}