
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef GLEW
#include <GL/glew.h>
#include <GL/gl.h>
#else
#include <epoxy/gl.h>
#endif

#include "phg.h"
#include "private/phgP.h"
#include "private/wsglP.h"

void wsgl_draw_vbo(GLenum mode, int dimensions, int count, const float *positions) {
    if (count <= 0) return;
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, count * dimensions * sizeof(float), positions, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, dimensions, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(dimensions, GL_FLOAT, 0, (void*)0);
    
    glDrawArrays(mode, 0, count);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}



static GLenum emu_mode = 0;
static float *emu_buffer = NULL;
static int emu_count = 0;
static int emu_capacity = 0;
static int emu_dimensions = 3;

void wsgl_begin_emu(GLenum mode) {
    emu_mode = mode;
    emu_count = 0;
    emu_dimensions = 3;
    if (emu_capacity == 0) {
        emu_capacity = 10000;
        emu_buffer = (float *)malloc(emu_capacity * 3 * sizeof(float));
    }
}

void wsgl_vertex2f_emu(float x, float y) {
    if (emu_count >= emu_capacity) {
        emu_capacity *= 2;
        emu_buffer = (float *)realloc(emu_buffer, emu_capacity * 3 * sizeof(float));
    }
    emu_buffer[emu_count * 3] = x;
    emu_buffer[emu_count * 3 + 1] = y;
    emu_buffer[emu_count * 3 + 2] = 0.0f;
    emu_count++;
}

void wsgl_vertex3f_emu(float x, float y, float z) {
    if (emu_count >= emu_capacity) {
        emu_capacity *= 2;
        emu_buffer = (float *)realloc(emu_buffer, emu_capacity * 3 * sizeof(float));
    }
    emu_buffer[emu_count * 3] = x;
    emu_buffer[emu_count * 3 + 1] = y;
    emu_buffer[emu_count * 3 + 2] = z;
    emu_count++;
}

void wsgl_end_emu(void) {
    if (emu_count > 0) {
        wsgl_draw_vbo(emu_mode, 3, emu_count, emu_buffer);
    }
}

void wsgl_vertex3dv_emu(const double *v) {
    wsgl_vertex3f_emu((float)v[0], (float)v[1], (float)v[2]);
}
