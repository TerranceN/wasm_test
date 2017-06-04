#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shader.h"
#include "math/mat4.h"

const float vertexBuf[] = {
  -0.0f,  0.5f, -1.0f,
  -0.5f, -0.5f, -1.0f,
   0.5f, -0.5f, -1.0f

  //1.0f,  0.0f,  0.0f,
  //0.0f,  1.0f,  0.0f,
  //0.0f,  0.0f,  1.0f
};

SDL_Window *window;
Shader shader;
bool isRunning = true;

int colorPos = -1;
float green = 0.0f;
float blue = 0.0f;

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
      green = mouseEvent->x/640.0f;
      blue = mouseEvent->y/480.0f;

      {
        mat4 mat = mat4_translate(mouseEvent->x, 480 - mouseEvent->y, 0.0);
        int modelMatrixLocation = glGetUniformLocation(shader.program, "u_ModelMatrix2");
        glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);
      }
    }
  }

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shader.program);
  glUniform2f(colorPos, green, blue);
  glDrawArrays(GL_TRIANGLES, 0, 3);

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

  glDisable(GL_DEPTH_TEST);
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

  printf("Version: %s\n", glGetString(GL_VERSION));

  shader = loadShaderFromFile("assets/shaders/simple.shader");

  glUseProgram(shader.program);

  {
    mat4 mat = mat4_orthographic(0, 640, 480, 0, 0, 1);
    mat4_print(&mat);
    int modelMatrixLocation = glGetUniformLocation(shader.program, "u_ProjectionMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);
  }
  {
    mat4 mat = mat4_scale(100.0, 100.0, 1.0);
    mat4_print(&mat);
    int modelMatrixLocation = glGetUniformLocation(shader.program, "u_ModelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);
  }
  {
    mat4 mat = mat4_identity();
    int modelMatrixLocation2 = glGetUniformLocation(shader.program, "u_ModelMatrix2");
    glUniformMatrix4fv(modelMatrixLocation2, 1, false, &mat.values[0]);
  }

  int buffer;
  glGenBuffers(1, (GLuint*)(&buffer));
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), vertexBuf, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  colorPos = glGetUniformLocation(shader.program, "color");

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
