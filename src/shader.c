#include <GL/glew.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "shader.h"

inline int max ( int a, int b ) { return a > b ? a : b; }
inline int min ( int a, int b ) { return a < b ? a : b; }

int compileShaderSrc(GLuint type, const char* src) {
  int shader = glCreateShader(type);
  int strLen = strlen(src);
  glShaderSource(shader, 1, &src, &strLen);
  glCompileShader(shader);

  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
    char infoLog[maxLength];
    glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

    printf("Failed to compile ");

    if (type == GL_VERTEX_SHADER) {
      printf("vertex");
    } else if (type == GL_FRAGMENT_SHADER) {
      printf("fragment");
    }

    printf(" shader: %s", infoLog);

    //We don't need the shader anymore.
    glDeleteShader(shader);
    return -1;
  }

  return shader;
}

Shader makeShader(const char* vertSrc, const char* fragSrc) {
  Shader shader;
  shader.program = -1;
  int vertexShader = compileShaderSrc(GL_VERTEX_SHADER, vertSrc);
  int fragmentShader = compileShaderSrc(GL_FRAGMENT_SHADER, fragSrc);

  // Link the two shaders into a program
  if (vertexShader >= 0 && fragmentShader >= 0) {
    int program;
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if(isLinked == GL_FALSE) {
      GLint maxLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

      //The maxLength includes the NULL character
      char infoLog[maxLength];
      glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

      //We don't need the program anymore.
      glDeleteProgram(program);
      //Don't leak shaders either.
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
    } else {
      shader.program = program;
    }
  }

  return shader;
}

// TODO: rewrite this to minify + parse parts of file in less hacky way
void loadShaderFile(const char* fileName, char** vert, char** frag) {
  FILE *file = fopen(fileName, "r");

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char line[256];

  *vert = malloc(fsize*sizeof(char));
  *frag = malloc(fsize*sizeof(char));
  char* vertp = *vert;
  char* fragp = *frag;

#ifdef __EMSCRIPTEN__
  sprintf(fragp, "precision highp float;\n");
  fragp += 23;
  sprintf(fragp, "#extension GL_OES_standard_derivatives : enable\n");
  fragp += 48;
#else
  sprintf(vertp, "#version 130\n");
  sprintf(fragp, "#version 130\n");
  vertp += 13;
  fragp += 13;
#endif

  int section = 0;

  while (fgets(line, sizeof(line), file)) {
    /* note that fgets don't strip the terminating \n, checking its
       presence would allow to handle lines longer that sizeof(line) */
    int len = strlen(line);

    if (line[0] == '}' && section > 0) {
      if (section == 1) {
        *vertp = '\0';
      } else if (section == 2) {
        *fragp = '\0';
      }
      section = -1;
      continue;
    } else if (strncmp("vertex_shader", line, 13) == 0) {
      section = 1;
      continue;
    } else if (strncmp("fragment_shader", line, 15) == 0) {
      section = 2;
      continue;
    }

    if (section == 0 || section == 1) {
      int offset = 0;
      if (section == 1) {
        offset = min(2, len-1);
      }
      memcpy(vertp, line+offset, (len-offset)*sizeof(char));
      vertp += (len-offset);
    }
    if (section == 0 || section == 2) {
      int offset = 0;
      if (section == 2) {
        offset = min(2, len-1);
      }
      memcpy(fragp, line+offset, (len-offset)*sizeof(char));
      fragp += (len-offset);
    }
  }
  /* may check feof here to make a difference between eof and io failure -- network
     timeout for instance */

  fclose(file);
}

Shader loadShaderFromFile(const char* fileName) {
  char* a;
  char* b;
  loadShaderFile(fileName, &a, &b);
  printf("---\nVERTEXSHADER\n---\n%s\n---\nFRAGMENTSHADER\n---\n%s\n", a, b);
  Shader shader = makeShader(a, b);
  free(a);
  free(b);
  return shader;
}
