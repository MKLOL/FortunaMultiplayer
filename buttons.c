/* 
* Author: Ioana-Maria Tamas
* https://github.com/ioanatamas/MusiTuna
* Enables creation of simple buttons with text, moving and selecting
*/

#include "buttons.h"
volatile rectangle R = {0,0,0,0};
uint16_t lefts[10], tops[10], button_col;
int16_t button_pos, last, i;
char* texts[10];

void init_buttons() {
    last = -1;
    button_pos = -1;
    button_col = BLUE;
}

void fill_rec(uint16_t l, uint16_t r, uint16_t t, uint16_t b, uint16_t col) { 
    R.left = l; 
    R.right = r; 
    R.top = t; 
    R.bottom = b; 
    fill_rectangle(R, col); 
}

void draw_button(uint16_t x) { 
    fill_rec(lefts[x],lefts[x]+66,tops[x],tops[x]+30,button_col);
    fill_rec(lefts[x]+3,lefts[x]+63,tops[x]+3,tops[x]+27,BLACK);
    fill_rec(lefts[x]+4,lefts[x]+62,tops[x]+4,tops[x]+26,button_col);
    display_color(WHITE,button_col);
    display_string_xy(texts[x],lefts[x]+7,tops[x]+12);
    display_color(WHITE,BLACK);
}

void make_button(uint16_t l, uint16_t t, char *text) {
    ++last;
    lefts[last] = l;
    tops[last] = t;
    texts[last] = text;
    button_col = BLUE;
    draw_button(last);
}

void select_button(uint16_t x) {
    button_col = CRIMSON;
    button_pos = x;
    draw_button(x);
}

void deselect_button(uint16_t x) {
    button_col = BLUE;
    button_pos = x;
    draw_button(x);
}

void deselect_all() {
    button_pos = -1;
    for(i=0; i<= last; ++i) {
        deselect_button(i);       
    }
}

uint16_t get_selected_button() {
    return button_pos;
}

void move_button_left() {
    if(button_pos > 0) {
        deselect_button(button_pos);
        select_button(button_pos-1);
    }
}

void move_button_right() {
    if(button_pos < last && button_pos != -1) {
        deselect_button(button_pos);
        select_button(button_pos+1);
    }
}

void reset_selections() {
    deselect_all();
    button_pos = -1;
}
