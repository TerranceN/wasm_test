#ifndef MAT4_H
#define MAT4_H

typedef struct {
  float values[16];
} mat4;

mat4 mat4_identity();
mat4 mat4_translate(float x, float y, float z);
mat4 mat4_scale(float x, float y, float z);

mat4 mat4_orthographic(float left, float right, float top, float bottom, float near, float far);

void mat4_print(mat4 *mat);

#endif
