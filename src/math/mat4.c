#include "mat4.h"

#include <stdio.h>

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
