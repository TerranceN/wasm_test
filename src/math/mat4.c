#include "mat4.h"

#include <stdio.h>
#include <math.h>

mat4 mat4_identity() {
  mat4 mat;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      mat.values[i*4+j] = i == j ? 1 : 0;
    }
  }
  return mat;
}

mat4 mat4_translate(float x, float y, float z) {
  mat4 mat = mat4_identity();
  mat.values[12] = x;
  mat.values[13] = y;
  mat.values[14] = z;
  return mat;
}

mat4 mat4_scale(float x, float y, float z) {
  mat4 mat = mat4_identity();
  mat.values[0] = x;
  mat.values[5] = y;
  mat.values[10] = z;
  return mat;
}

mat4 mat4_rotateX(float angle) {
  mat4 mat = mat4_identity();
  float c = cos(angle);
  float s = sin(angle);
  mat.values[5] = c;
  mat.values[6] = s;
  mat.values[9] = -s;
  mat.values[10] = c;
  return mat;
}

mat4 mat4_rotateY(float angle) {
  mat4 mat = mat4_identity();
  float c = cos(angle);
  float s = sin(angle);
  mat.values[0] = c;
  mat.values[2] = -s;
  mat.values[8] = s;
  mat.values[10] = c;
  return mat;
}

mat4 mat4_rotateZ(float angle) {
  mat4 mat = mat4_identity();
  float c = cos(angle);
  float s = sin(angle);
  mat.values[0] = c;
  mat.values[1] = s;
  mat.values[4] = -s;
  mat.values[5] = c;
  return mat;
}

mat4 mat4_orthographic(float left, float right, float top, float bottom, float near, float far) {
  mat4 mat = mat4_identity();
  float xDiff = right - left;
  float yDiff = top - bottom;
  float zDiff = far - near;
  mat.values[0] = 2/xDiff;
  mat.values[5] = 2/yDiff;
  mat.values[10] = -2/zDiff;
  mat.values[12] = -(right + left)/xDiff;
  mat.values[13] = -(top + bottom)/yDiff;
  mat.values[14] = -(far + near)/zDiff;
  return mat;
}

mat4 mat4_perspective(float fovy, float aspect, float near, float far) {
  mat4 mat = mat4_identity();
  float f = 1.0/tan(fovy/2);
  float zDiff = near - far;
  mat.values[0] = f/aspect;
  mat.values[5] = f;
  mat.values[10] = (far+near)/zDiff;
  mat.values[11] = -1;
  mat.values[14] = (2*far*near)/zDiff;
  mat.values[15] = 0;
  return mat;
}

void mat4_multiply_ptr(mat4 *dst, mat4 *m1, mat4 *m2) {
  for (int j = 0; j < 4; j++) { // y-index
    for (int i = 0; i < 4; i++) { // x-index
      float total = 0;
      for (int k = 0; k < 4; k++) {
        total += m1->values[k*4+j] * m2->values[i*4+k];
      }
      dst->values[i*4+j] = total;
    }
  }
}

mat4 mat4_multiply(mat4 *m1, mat4 *m2) {
  mat4 mat;
  mat4_multiply_ptr(&mat, m1, m2);
  return mat;
}

void mat4_print(mat4 *mat) {
  printf("[\t");
  for (int j = 0; j < 4; j++) { // y-index
    for (int i = 0; i < 4; i++) { // x-index
      printf("%f\t", mat->values[i*4+j]);
    }
    printf("\n");
    if (j < 3) {
      printf(".\t");
    }
  }
  printf("]\n");
}
