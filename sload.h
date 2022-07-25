#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

long sl_compile_link_file(const char* vertex_shader_source_path, const char* fragment_shader_source_path);
long sl_compile_link_src(const char* vertex_shader_source, const char* fragment_shader_source);

#ifdef SLOAD_IMPLEMENTATION

#ifndef WINDOWS
	#include <GLES3/gl3.h>
#else
	#include <GL/glew.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "assetloader.h"

static const char * const shader_names[] = {"vertex", "fragment"};    
static const unsigned int shader_types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

long sl_compile_link_src(const char* vertex_shader_source, const char* fragment_shader_source) {
    unsigned int shader_ids[2] = {0};        
    long shader_program = -1;
	const char* shader_srcs[2] = {vertex_shader_source, fragment_shader_source};

    for(int i = 0; i < 2; i++) {
        unsigned int shader_id = glCreateShader(shader_types[i]);
        glShaderSource(shader_id, 1, shader_srcs+i, NULL);
        glCompileShader(shader_id);

        int shader_compilation_status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compilation_status); //compilation success status
        if(!shader_compilation_status) { //check for bad compilation
            char compilation_info[1024] = {0};
            glGetShaderInfoLog(shader_id, 1023, NULL, compilation_info);
            fprintf(stderr, "%s shader compilation failed: -> %s\n", shader_names[i], compilation_info);
            goto dispose;
        }
        shader_ids[i] = shader_id;    
    } 
    
    //linking shaders into a shader program
    shader_program = glCreateProgram();
    for(int i = 0; i < 2; i++) {
        glAttachShader(shader_program, shader_ids[i]);
    }
    glLinkProgram(shader_program);

    int shader_linking_status;
	    glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_linking_status);
    if(!shader_linking_status) { //check for bad linking
        char linking_info[1024] = {0};
        glGetProgramInfoLog(shader_program, 1023, NULL, linking_info);
        fprintf(stderr, "Shader linking failed: -> %s\n", linking_info);
    }
    
    dispose:
    for(int i = 0; i < 2; i++) {
        if(!shader_ids[i])
            break;
        glDeleteShader(shader_ids[i]);
    }    
    return shader_program;        
}

long sl_compile_link_file(const char* vertex_shader_source_path, const char* fragment_shader_source_path) {
	char* shader_srcs[2];
	long tmp;
	shader_srcs[0] = load_asset((char*)vertex_shader_source_path, &tmp);
	shader_srcs[1] = load_asset((char*)fragment_shader_source_path, &tmp);
	
	long result = sl_compile_link_src(shader_srcs[0], shader_srcs[1]);
	if(result < 0) {
		//LOGI("Error in shader files: (%s), (%s)\n", vertex_shader_source_path, fragment_shader_source_path);
		//LOGI("vs_src: _%s_\nfs_src: _%s_\n", shader_srcs[0], shader_srcs[1]);
	}
	
	free(shader_srcs[0]);
	free(shader_srcs[1]);

	return result;
}
#endif
#endif
