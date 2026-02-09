#ifndef KEYPAD_H
#define KEYPAD_H

#include <avr/io.h>
#include <util/delay.h>

// Bản đồ bàn phím 4x4
extern char keymap[4][4];

// Khai báo hàm
void keypad_init(void);
char read_keypad(void);

#endif
