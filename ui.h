#ifndef UI_H
#define UI_H

#include <stdint.h>
#include "brender.h"

int  ui_checkbox(int posx, int posy, int size, char* text, int selected);
int  ui_button(int posx, int posy, int size, char* text);
void ui_set_input(int mouse_x, int mouse_y, int mouse_down);
void ui_update();

#ifdef UI_IMPLEMENTATION

typedef struct {
	int mouse_x;
	int mouse_y;
	int mouse_down;
}UI_Input;
static UI_Input ui_input;
static UI_Input ui_prev_input;

static int in_rect(int px, int py, int x0, int y0, int x1, int y1) {
	if(px < x0 || px > x1)
		return 0;
	if(py < y0 || py > y1)
		return 0;
	return 1;	
}

void ui_update() {
	ui_prev_input = ui_input;	//handle input
}
void ui_set_input(int mouse_x, int mouse_y, int mouse_down) {
	ui_prev_input = ui_input;	
	ui_input.mouse_down = mouse_down;
	ui_input.mouse_x = mouse_x;
	ui_input.mouse_y = mouse_y;
}
int ui_checkbox(int posx, int posy, int size, char* text, int selected) {
	int hover = in_rect(ui_input.mouse_x, ui_input.mouse_y, posx, posy, posx+size, posy+size);
	int mouse_down = hover && ui_input.mouse_down;
	int click = !ui_prev_input.mouse_down /*&& prev_hover*/ && mouse_down;

	//1. rob	
	if(hover)
		br_set_rect_color(0.5f,0,0);
	else
		br_set_rect_color(0,0,0);
		
	br_draw_rect(posx, posy, posx+size, posy+size);

	int thickness = 3;
	size -= 2*thickness;
	posx += thickness;
	posy += thickness;

	//notranji	
	if(selected)
		br_set_rect_color(0.7f,0.2f,0);
	else
		br_set_rect_color(0.2f, 0.2f, 0.2f);

	br_set_rect_alpha(0.7f);
	br_draw_rect(posx, posy, posx+size, posy+size);
	br_set_text_size(size*0.5f, size);
	br_draw_text(posx + size + 10, posy, text);

	return click != selected;
}
int ui_button(int posx, int posy, int size, char* text) {
	int len = strlen(text);
	int padding = 4;
	int border_thickness = 3;

	float text_size_y = size - 2.0f*(border_thickness + padding);
	float text_size_x = 0.5f * text_size_y;

	float rect_size_x = text_size_x*len + 2*padding;
	float rect_size_y = text_size_y     + 2*padding;

	float border_size_x = rect_size_x + 2*border_thickness;
	float border_size_y = rect_size_y + 2*border_thickness;

	int hover = in_rect(ui_input.mouse_x, ui_input.mouse_y, posx, posy, posx+border_size_x, posy+border_size_y);
	int mouse_down = hover && ui_input.mouse_down;
	int click = !ui_prev_input.mouse_down /*&& prev_hover*/ && mouse_down;

	if(hover)
		br_set_rect_color(0.5f,0,0);
	else
		br_set_rect_color(0,0,0);

	br_set_rect_alpha(0.7f);
	br_draw_rect(posx, posy, posx + border_size_x, posy + border_size_y);

	posx += border_thickness;
	posy += border_thickness;

	if(mouse_down) {
		br_set_rect_color(0.7f,0.2f,0);
	}
	else
		br_set_rect_color(0.2f, 0.2f, 0.2f);
	br_draw_rect(posx, posy, posx + rect_size_x, posy + rect_size_y);
	
	posx += padding;
	posy += padding;

	br_set_text_color(1,1,1);
	br_set_text_size(text_size_x, text_size_y);
	br_draw_text(posx, posy, text);

	return click;
}

#endif
#endif
