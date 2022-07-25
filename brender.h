#ifndef B_RENDER_H
#define B_RENDER_H

#include "bint.h"

void br_init(int width, int height);
void br_resize_screen(int width, int height);

void br_draw_rect(int x0, int y0, int x1, int y1); /* spodnja diagonala (x0, y0), zgornja diagonala (x1, y1) */
void br_draw_rect_full(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);
void br_draw_line(int x0, int y0, int x1, int y1, float thickness);
void br_draw_circle(int x, int y, float r, float edge_thickness);
void br_draw_text(int posx, int posy, char* text);
void br_draw_triangle();

void br_set_text_size(float w, float h);
void br_set_circle_color(float r, float g, float b);
void br_set_rect_color(float r, float g, float b);
void br_set_rect_alpha(float alpha);
void br_set_text_color(float r, float g, float b);

#ifdef BRENDER_IMPLEMENTATION

#ifndef WINDOWS
	#include <GLES3/gl3.h>
#else
	#include <GL/glew.h>
#endif
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "sload.h"
#include "ppmloader.h"

static struct {
	int width;
	int height;
	float aspect;
}r_data;

static struct {
	uint32_t vao;
	uint32_t shader;
}tri_r;

static struct {
	uint32_t vao;
	uint32_t vbo;
	uint32_t shader;

	uint32_t uloc_color;
	Vec4f color;
}rect_r;

static struct {
	uint32_t vao;
	uint32_t vbo;
	uint32_t shader;

	uint32_t uloc_center;
	uint32_t uloc_radius;
	uint32_t uloc_thickness;
	uint32_t uloc_color;

	Vec3f color;
}circle_r;

static struct {
	uint32_t vao;
	uint32_t quad_vbo;
	uint32_t glyph_vbo;
	uint32_t texture;
	uint32_t shader;

	uint32_t uloc_color;
	uint32_t uloc_position_origin;
	Vec3f color;
	Vec2f size;
}text_r;

void br_resize_screen(int width, int height) {
	r_data.width = width;
	r_data.height = height;
	r_data.aspect = (float)width / height;
	
	br_set_text_size(text_r.size.x, text_r.size.y);
}

void br_set_circle_color(float r, float g, float b) {
	circle_r.color = (Vec3f){r,g,b};
}
void br_set_rect_color(float r, float g, float b) {
	rect_r.color = (Vec4f){r,g,b, rect_r.color.w}; //keep alpha
}
void br_set_rect_alpha(float alpha) {
	rect_r.color.w = alpha;
}
void br_set_text_color(float r, float g, float b) {
	text_r.color = (Vec3f){r,g,b};
}
void br_set_text_size(float w, float h) {
	text_r.size = (Vec2f){w,h};
	w*=2;
	h*=2;
	w = w/r_data.width;
	h = h/r_data.height;
	float quad[] = {
		0,0,
		0,h,
		w,0,
		w,h
	};
	glBindBuffer(GL_ARRAY_BUFFER, text_r.quad_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  sizeof(quad), quad);
}

void br_init(int width, int height) {
	br_resize_screen(width, height);
	{
		glGenVertexArrays(1, &tri_r.vao);
		tri_r.shader = sl_compile_link_file("a.vs", "b.fs");
	}
	{
		glGenVertexArrays(1, &rect_r.vao);
		glBindVertexArray(rect_r.vao);
	
		glGenBuffers(1, &rect_r.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, rect_r.vbo);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		
		rect_r.shader = sl_compile_link_file("rect.vs", "rect.fs");

		rect_r.uloc_color = glGetUniformLocation(rect_r.shader, "color");
	}
	{
		glGenVertexArrays(1, &circle_r.vao);
		glBindVertexArray(circle_r.vao);
	
		glGenBuffers(1, &circle_r.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, circle_r.vbo);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		
		//circle_r.shader = sl_compile_link_file("rect.vs", "circle.fs");
		circle_r.shader = sl_compile_link_file("rect.vs", "circle.fs");

		circle_r.uloc_center 	= glGetUniformLocation(circle_r.shader, "center");
		circle_r.uloc_radius 	= glGetUniformLocation(circle_r.shader, "radius");
		circle_r.uloc_thickness = glGetUniformLocation(circle_r.shader, "thickness");
		circle_r.uloc_color 	= glGetUniformLocation(circle_r.shader, "color");
	}
	{
		glGenVertexArrays(1, &text_r.vao);
		glBindVertexArray(text_r.vao);
		
		glGenBuffers(1, &text_r.quad_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, text_r.quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), NULL, GL_STATIC_DRAW);	
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		//zadnja dva elementa sta size
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6*(sizeof(float))));
		glEnableVertexAttribArray(1);
		glVertexAttribDivisor(1, -1);


		glGenBuffers(1, &text_r.glyph_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, text_r.glyph_vbo);
		glBufferData(GL_ARRAY_BUFFER, 1000*sizeof(float), NULL, GL_DYNAMIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(tmp), tmp, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)offsetof(Vec3f, x));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)offsetof(Vec3f, y));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)offsetof(Vec3f, z));
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		//glyph_index se nastavi na instanco, ne na vsak vertex.
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		//text shader
		text_r.shader = sl_compile_link_file("text.vs", "text.fs");

		//nalozimo texturo
		PPM_Image* font_atlas =  il_load("font_atlas.ppm");
		glGenTextures(1, &text_r.texture);	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, text_r.texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, font_atlas->width,
			font_atlas->height, 0, GL_RGB, GL_UNSIGNED_BYTE, font_atlas->data);
			
		glGenerateMipmap(GL_TEXTURE_2D);	
		glUniform1i(glGetUniformLocation(text_r.shader, "font-atlas"), 0);

		il_img_free(font_atlas);

		text_r.uloc_color = glGetUniformLocation(text_r.shader, "color");
		text_r.uloc_position_origin = glGetUniformLocation(text_r.shader, "origin");
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}

void br_draw_text(int posx, int posy, char* text) {
	glUseProgram(text_r.shader);
	glBindVertexArray(text_r.vao);
	glUniform3f(text_r.uloc_color, text_r.color.x, text_r.color.y, text_r.color.z);

	float nx = 2.0f*posx/r_data.width - 1.0f;
	float ny = 2.0f*posy/r_data.height - 1.0f;

	glUniform2f(text_r.uloc_position_origin, nx, ny);

	int len = strlen(text);
	if(len > 1000)
		len = 1000;

	Vec3f glyph_infos[1000];
	int px = 0;
	int py = 0;
	for(int i = 0; i < len; ++i) {
		char c = text[i];
		int glyph_index = c-' ';
		if(c == '\n') {
			py -= 1;
			px = 0;
			continue;
		}
		glyph_infos[i] = (Vec3f){glyph_index, px, py};
		px += 1;
	}

	glBindBuffer(GL_ARRAY_BUFFER, text_r.glyph_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, len*sizeof(Vec3f), glyph_infos);

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
}

void br_draw_circle(int x, int y, float r, float edge_thickness) {
	float x0n = 2.0f* ( x - r*1.1f ) / r_data.width  - 1.0f;
	float x1n = 2.0f* ( x + r*1.1f ) / r_data.width  - 1.0f;
	float y0n = 2.0f* ( y - r*1.1f ) / r_data.height - 1.0f;
	float y1n = 2.0f* ( y + r*1.1f ) / r_data.height - 1.0f;

	glUseProgram(circle_r.shader);
	glBindVertexArray(circle_r.vao);

	glUniform2f(circle_r.uloc_center, x, y);
	glUniform1f(circle_r.uloc_radius, r);
	glUniform1f(circle_r.uloc_thickness, edge_thickness);
	glUniform3f(circle_r.uloc_color, circle_r.color.x, circle_r.color.y, circle_r.color.z);

	float circle_vtx[] = {
		x1n, y0n,
		x1n, y1n,
		x0n, y0n, 
		x0n ,y1n
	};
	glBindBuffer(GL_ARRAY_BUFFER, circle_r.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*sizeof(float), circle_vtx);	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void br_draw_line(int x0, int y0, int x1, int y1, float thickness) {
	float dist = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));

	float orto_x = (y1 - y0) / dist;
	float orto_y = (x0 - x1) / dist;
	float th2 = thickness/2.0f;

	br_draw_rect_full(
		x0 - orto_x*th2, y0 - orto_y*th2,
		x1 - orto_x*th2, y1 - orto_y*th2,
		x1 + orto_x*th2, y1 + orto_y*th2,
		x0 + orto_x*th2, y0 + orto_y*th2
	);
}

void br_draw_rect_full(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
	glUseProgram(rect_r.shader);
	glBindVertexArray(rect_r.vao);

	float x0n = 2.0f * (float)x0 / r_data.width - 1.0f;
	float x1n = 2.0f * (float)x1 / r_data.width - 1.0f;
	float x2n = 2.0f * (float)x2 / r_data.width - 1.0f;
	float x3n = 2.0f * (float)x3 / r_data.width - 1.0f;

	float y0n = 2.0f * (float)y0 / r_data.height - 1.0f;
	float y1n = 2.0f * (float)y1 / r_data.height - 1.0f;
	float y2n = 2.0f * (float)y2 / r_data.height - 1.0f;
	float y3n = 2.0f * (float)y3 / r_data.height - 1.0f;

	float rect_vtx[] = {
		x0n, y0n,
		x1n, y1n,
		x3n, y3n,
		x2n, y2n
	};
	glBindBuffer(GL_ARRAY_BUFFER, rect_r.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*sizeof(float), rect_vtx);	

	glUniform4f(rect_r.uloc_color, rect_r.color.x, rect_r.color.y, rect_r.color.z, rect_r.color.w);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void br_draw_rect(int x0, int y0, int x1, int y1) {
	glUseProgram(rect_r.shader);
	glBindVertexArray(rect_r.vao);

	float x0n = 2.0f * (float)x0 / r_data.width - 1.0f;
	float x1n = 2.0f * (float)x1 / r_data.width - 1.0f;

	float y0n = 2.0f * (float)y0 / r_data.height - 1.0f;
	float y1n = 2.0f * (float)y1 / r_data.height - 1.0f;

	float rect_vtx[] = {
		x1n, y0n,
		x1n, y1n,
		x0n, y0n, 
		x0n ,y1n
	};
	glBindBuffer(GL_ARRAY_BUFFER, rect_r.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*sizeof(float), rect_vtx);	

	glUniform4f(rect_r.uloc_color, rect_r.color.x, rect_r.color.y, rect_r.color.z, rect_r.color.w);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

//demo test feature
void br_draw_triangle() {
	glUseProgram(tri_r.shader);
	glBindVertexArray(tri_r.vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

#endif
#endif
