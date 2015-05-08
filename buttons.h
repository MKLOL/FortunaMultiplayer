/* 
* Author: Ioana-Maria Tamas
* https://github.com/ioanatamas/MusiTuna
* Enables creation of simple buttons with text, moving and selecting
*/
#include "lcd.h"

void init_buttons();
void fill_rec(uint16_t l, uint16_t r, uint16_t t, uint16_t b, uint16_t col);
void make_button(uint16_t l, uint16_t t, char *text);
void select_button(uint16_t x);
void move_button_left();
void move_button_right();
void reset_selections();
uint16_t get_selected_button();
