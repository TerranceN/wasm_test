#ifndef SHADER_H
#define SHADER_H

typedef struct {
  int program;
} Shader;

Shader loadShaderFromFile(const char* fileName);

#endif
