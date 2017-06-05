#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shader.h"
#include "math/mat4.h"

const float triangleVertexBuf[] = {
  -0.0f,  0.5f, -1.0f,
  -0.5f, -0.5f, -1.0f,
   0.5f, -0.5f, -1.0f

  //1.0f,  0.0f,  0.0f,
  //0.0f,  1.0f,  0.0f,
  //0.0f,  0.0f,  1.0f
};

const float cubeVertexBuf[] = {
  // Back
   0.5f,  0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,

  -0.5f,  0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,

  // Front
   0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f, -0.5f,  0.5f,

  -0.5f, -0.5f,  0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,

  // Right
   0.5f,  0.5f,  0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f, -0.5f, -0.5f,

   0.5f, -0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
   0.5f,  0.5f,  0.5f,

  // Left
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,

  -0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,

  // Top
   0.5f,  0.5f,  0.5f,
   0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f,  0.5f,

  -0.5f,  0.5f,  0.5f,
   0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,

  // Bottom
  -0.5f, -0.5f,  0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f, -0.5f,  0.5f,

  -0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
};

int triangleVAO;
int cubeVAO;

SDL_Window *window;
Shader shader;
bool isRunning = true;

float triangleX;
float triangleY;
float triangleScale = 100;

mat4 cubeModelMatrix;;

void one_iter() {
  SDL_Event event;
  while (SDL_PollEvent(&event)){
    //If user closes the window
    if (event.type == SDL_QUIT){
      isRunning = false;
    }
    //If user presses any key
    if (event.type == SDL_KEYDOWN){
      isRunning = false;
    }

    if (event.type == SDL_MOUSEMOTION) {
      SDL_MouseMotionEvent *mouseEvent = (SDL_MouseMotionEvent*)(&event);

      triangleX = mouseEvent->x;
      triangleY = 480 - mouseEvent->y;
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader.program);
  {
    int modelMatrixLocation = glGetUniformLocation(shader.program, "u_ModelMatrix");
    int projectionMatrixLocation = glGetUniformLocation(shader.program, "u_ProjectionMatrix");

    {
      mat4 ortho = mat4_orthographic(0, 640, 480, 0, 0, 1);
      glUniformMatrix4fv(projectionMatrixLocation, 1, false, &ortho.values[0]);

      {
        mat4 scale = mat4_scale(triangleScale, triangleScale, 0.0);
        mat4 translate = mat4_translate(triangleX, triangleY, 0.0);
        mat4 mat = mat4_multiply(&translate, &scale);
        glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
      }

    }
    {
      mat4 rot;
      rot = mat4_rotateY(0.03);
      cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      rot = mat4_rotateX(0.01);
      cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      rot = mat4_rotateZ(0.02);
      cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      mat4 persp = mat4_perspective(90, 640.0/480.0, 0.1, 100.0);
      glUniformMatrix4fv(projectionMatrixLocation, 1, false, &persp.values[0]);
      {
        glUniformMatrix4fv(modelMatrixLocation, 1, false, &cubeModelMatrix.values[0]);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
  }

  SDL_GL_SwapWindow(window);
}

#ifdef __EMSCRIPTEN__
void emscripten_set_main_loop(void*, int, int);
#endif

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0){
    return 1;
  }

  window = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (window == NULL){
    SDL_Quit();
    return 1;
  }
  //
  // Create an OpenGL context associated with the window.
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);
  glewInit();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

  printf("Version: %s\n", glGetString(GL_VERSION));

  shader = loadShaderFromFile("assets/shaders/simple.shader");

  glUseProgram(shader.program);

  {
    mat4 scale = mat4_scale(1.0, 1.0, 1.0);
    mat4 translate = mat4_translate(0, 0, -2.5);
    cubeModelMatrix = mat4_multiply(&translate, &scale);
  }

  {
    int buffer;
    glGenVertexArrays(1, (GLuint*)(&triangleVAO));
    glBindVertexArray(triangleVAO);
    glGenBuffers(1, (GLuint*)(&buffer));
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), triangleVertexBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
  }

  {
    int buffer;
    glGenVertexArrays(1, (GLuint*)(&cubeVAO));
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, (GLuint*)(&buffer));
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 108*sizeof(float), cubeVertexBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
  }

#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
  emscripten_set_main_loop(one_iter, 0, 1);
#else
  while (isRunning) {
    one_iter();
    SDL_Delay(17);
  }
#endif

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
