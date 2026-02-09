#include "keypad.h"

char keymap[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'L','0','R','D'}  // L = Shift trái, R = Shift phải, D = OK
};

void keypad_init(void) {
	DDRD = 0x0F;    // 4 bit thấp (PD0–PD3) làm ngõ ra – hàng
	PORTD = 0xF0;   // 4 bit cao (PD4–PD7) kéo lên – cột
}

char read_keypad(void) {
	for (uint8_t row = 0; row < 4; row++) {
		// Xuất mức thấp cho từng hàng, các hàng khác mức cao
		PORTD = (~(1 << row) & 0x0F) | 0xF0;
		_delay_ms(1);

		for (uint8_t col = 0; col < 4; col++) {
			if (!(PIND & (1 << (col + 4)))) { // Cột được nhấn
				_delay_ms(20); // debounce
				if (!(PIND & (1 << (col + 4)))) {
					while (!(PIND & (1 << (col + 4)))); // đợi nhả phím
					_delay_ms(20);
					return keymap[row][col];
				}
			}
		}
	}
	return 0; // Không nhấn phím
}
