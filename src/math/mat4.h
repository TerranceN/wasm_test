#ifndef MAT4_H
#define MAT4_H

typedef struct {
  float values[16];
} mat4;

mat4 mat4_identity();
mat4 mat4_translate(float x, float y, float z);
mat4 mat4_scale(float x, float y, float z);
mat4 mat4_rotateX(float angle);
mat4 mat4_rotateY(float angle);
mat4 mat4_rotateZ(float angle);

mat4 mat4_orthographic(float left, float right, float top, float bottom, float near, float far);
mat4 mat4_perspective(float fovy, float aspect, float near, float far);

void mat4_multiply_ptr(mat4 *dst, mat4 *m1, mat4 *m2);
mat4 mat4_multiply(mat4 *m1, mat4 *m2);

void mat4_print(mat4 *mat);

#endif
