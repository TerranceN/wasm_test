#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shader.h"
#include "math/mat4.h"

#define numElems(array) (sizeof(array) == 0 ? 0 : sizeof(array) / sizeof(array[0]));

const float triangleVertexBuf[] = {
  -0.0f,  0.5f, -1.0f,
  -0.5f, -0.5f, -1.0f,
   0.5f, -0.5f, -1.0f

  //1.0f,  0.0f,  0.0f,
  //0.0f,  1.0f,  0.0f,
  //0.0f,  0.0f,  1.0f
};
const int triangleBufElems = numElems(triangleVertexBuf);

const float cubeVertexBuf[] = {
  // Back
   0.4f,  0.4f, -0.5f,
   0.4f, -0.4f, -0.5f,
  -0.4f,  0.4f, -0.5f,

  -0.4f,  0.4f, -0.5f,
   0.4f, -0.4f, -0.5f,
  -0.4f, -0.4f, -0.5f,

  // Front
   0.4f,  0.4f,  0.5f,
  -0.4f,  0.4f,  0.5f,
  -0.4f, -0.4f,  0.5f,

  -0.4f, -0.4f,  0.5f,
   0.4f, -0.4f,  0.5f,
   0.4f,  0.4f,  0.5f,

  // Right
   0.5f,  0.4f,  0.4f,
   0.5f, -0.4f,  0.4f,
   0.5f, -0.4f, -0.4f,

   0.5f, -0.4f, -0.4f,
   0.5f,  0.4f, -0.4f,
   0.5f,  0.4f,  0.4f,

  // Left
  -0.5f, -0.4f, -0.4f,
  -0.5f, -0.4f,  0.4f,
  -0.5f,  0.4f,  0.4f,

  -0.5f,  0.4f,  0.4f,
  -0.5f,  0.4f, -0.4f,
  -0.5f, -0.4f, -0.4f,

  // Top
   0.4f,  0.5f,  0.4f,
   0.4f,  0.5f, -0.4f,
  -0.4f,  0.5f,  0.4f,

  -0.4f,  0.5f,  0.4f,
   0.4f,  0.5f, -0.4f,
  -0.4f,  0.5f, -0.4f,

  // Bottom
  -0.4f, -0.5f,  0.4f,
   0.4f, -0.5f, -0.4f,
   0.4f, -0.5f,  0.4f,

  -0.4f, -0.5f, -0.4f,
   0.4f, -0.5f, -0.4f,
  -0.4f, -0.5f,  0.4f,

  // Front-Top connector

  -0.4f,  0.5f,  0.4f,
  -0.4f,  0.4f,  0.5f,
   0.4f,  0.5f,  0.4f,

   0.4f,  0.5f,  0.4f,
  -0.4f,  0.4f,  0.5f,
   0.4f,  0.4f,  0.5f,

  // Front-Bottom connector

   0.4f, -0.5f,  0.4f,
  -0.4f, -0.4f,  0.5f,
  -0.4f, -0.5f,  0.4f,

   0.4f, -0.4f,  0.5f,
  -0.4f, -0.4f,  0.5f,
   0.4f, -0.5f,  0.4f,

  // Front-Left connector

  -0.5f,  0.4f,  0.4f,
  -0.5f, -0.4f,  0.4f,
  -0.4f, -0.4f,  0.5f,

  -0.4f, -0.4f,  0.5f,
  -0.4f,  0.4f,  0.5f,
  -0.5f,  0.4f,  0.4f,

  // Front-Right connector

   0.4f, -0.4f,  0.5f,
   0.5f, -0.4f,  0.4f,
   0.5f,  0.4f,  0.4f,

   0.5f,  0.4f,  0.4f,
   0.4f,  0.4f,  0.5f,
   0.4f, -0.4f,  0.5f,

  // Back-Top connector

   0.4f,  0.5f, -0.4f,
  -0.4f,  0.4f, -0.5f,
  -0.4f,  0.5f, -0.4f,

   0.4f,  0.4f, -0.5f,
  -0.4f,  0.4f, -0.5f,
   0.4f,  0.5f, -0.4f,

  // Back-Bottom connector

  -0.4f, -0.5f, -0.4f,
  -0.4f, -0.4f, -0.5f,
   0.4f, -0.5f, -0.4f,

   0.4f, -0.5f, -0.4f,
  -0.4f, -0.4f, -0.5f,
   0.4f, -0.4f, -0.5f,

  // Back-Left connector

  -0.4f, -0.4f, -0.5f,
  -0.5f, -0.4f, -0.4f,
  -0.5f,  0.4f, -0.4f,

  -0.5f,  0.4f, -0.4f,
  -0.4f,  0.4f, -0.5f,
  -0.4f, -0.4f, -0.5f,

  // Back-Right connector

   0.5f,  0.4f, -0.4f,
   0.5f, -0.4f, -0.4f,
   0.4f, -0.4f, -0.5f,

   0.4f, -0.4f, -0.5f,
   0.4f,  0.4f, -0.5f,
   0.5f,  0.4f, -0.4f,

   // Left-Top connector

  -0.4f,  0.5f, -0.4f,
  -0.5f,  0.4f, -0.4f,
  -0.5f,  0.4f,  0.4f,

  -0.5f,  0.4f,  0.4f,
  -0.4f,  0.5f,  0.4f,
  -0.4f,  0.5f, -0.4f,

   // Left-Bottom connector

  -0.5f, -0.4f,  0.4f,
  -0.5f, -0.4f, -0.4f,
  -0.4f, -0.5f, -0.4f,

  -0.4f, -0.5f, -0.4f,
  -0.4f, -0.5f,  0.4f,
  -0.5f, -0.4f,  0.4f,

   // Right-Top connector

   0.5f,  0.4f,  0.4f,
   0.5f,  0.4f, -0.4f,
   0.4f,  0.5f, -0.4f,

   0.4f,  0.5f, -0.4f,
   0.4f,  0.5f,  0.4f,
   0.5f,  0.4f,  0.4f,

   // Right-Bottom connector

   0.4f, -0.5f, -0.4f,
   0.5f, -0.4f, -0.4f,
   0.5f, -0.4f,  0.4f,

   0.5f, -0.4f,  0.4f,
   0.4f, -0.5f,  0.4f,
   0.4f, -0.5f, -0.4f,

   // Front-Top-Left corner

  -0.4f,  0.5f,  0.4f,
  -0.5f,  0.4f,  0.4f,
  -0.4f,  0.4f,  0.5f,

   // Front-Top-Right corner

   0.4f,  0.4f,  0.5f,
   0.5f,  0.4f,  0.4f,
   0.4f,  0.5f,  0.4f,

   // Front-Bottom-Left corner

  -0.4f, -0.4f,  0.5f,
  -0.5f, -0.4f,  0.4f,
  -0.4f, -0.5f,  0.4f,

   // Front-Bottom-Right corner

   0.4f, -0.5f,  0.4f,
   0.5f, -0.4f,  0.4f,
   0.4f, -0.4f,  0.5f,

   // Back-Top-Left corner

  -0.4f,  0.4f, -0.5f,
  -0.5f,  0.4f, -0.4f,
  -0.4f,  0.5f, -0.4f,

   // Back-Top-Right corner

   0.4f,  0.5f, -0.4f,
   0.5f,  0.4f, -0.4f,
   0.4f,  0.4f, -0.5f,

   // Back-Bottom-Left corner

  -0.4f, -0.5f, -0.4f,
  -0.5f, -0.4f, -0.4f,
  -0.4f, -0.4f, -0.5f,

   // Back-Bottom-Right corner

   0.4f, -0.4f, -0.5f,
   0.5f, -0.4f, -0.4f,
   0.4f, -0.5f, -0.4f,
};
const int cubeBufElems = numElems(cubeVertexBuf);

int triangleVAO;
int cubeVAO;

SDL_Window *window;
Shader shader;
Shader flatShader;
bool isRunning = true;

float triangleX;
float triangleY;
float triangleScale = 100;

mat4 cubeModelMatrix;

float tetronimo[] = {
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
};

void one_iter() {
  SDL_Event event;
  while (SDL_PollEvent(&event)){
    //If user closes the window
    if (event.type == SDL_QUIT){
      isRunning = false;
    }
    //If user presses any key
    if (event.type == SDL_KEYDOWN){
      SDL_KeyboardEvent *keyEvent = (SDL_KeyboardEvent*)(&event);

      if (keyEvent->type == SDL_KEYDOWN && keyEvent->keysym.sym == SDLK_ESCAPE) {
        isRunning = false;
      }

      if (keyEvent->type == SDL_KEYDOWN && keyEvent->keysym.sym == SDLK_r) {
        mat4_transpose_in_place((mat4*)tetronimo);
        mat4_flip_horiz_in_place((mat4*)tetronimo);
      }
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
    int modelMatrixLocation = glGetUniformLocation(shader.program, "u_modelMatrix");
    int projectionMatrixLocation = glGetUniformLocation(shader.program, "u_projectionMatrix");

    {
      mat4 ortho = mat4_orthographic(0, 640, 480, 0, 0, 1);
      glUniformMatrix4fv(projectionMatrixLocation, 1, false, &ortho.values[0]);

      {
        mat4 scale = mat4_scale(triangleScale, triangleScale, 0.0);
        mat4 translate = mat4_translate(triangleX, triangleY, 0.0);
        mat4 mat = mat4_multiply(&translate, &scale);
        glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, triangleBufElems/3);
      }

    }
  }
  glUseProgram(flatShader.program);
  {
    int modelMatrixLocation = glGetUniformLocation(flatShader.program, "u_modelMatrix");
    int viewMatrixLocation = glGetUniformLocation(flatShader.program, "u_viewMatrix");
    int projectionMatrixLocation = glGetUniformLocation(flatShader.program, "u_projectionMatrix");

    {
      mat4 mat = mat4_translate(0, 0, -6);
      glUniformMatrix4fv(viewMatrixLocation, 1, false, &mat.values[0]);
    }

    {
      mat4 rot;
      rot = mat4_rotateY(0.03);
      cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      //rot = mat4_rotateX(0.01);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      //rot = mat4_rotateZ(0.02);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      mat4 persp = mat4_perspective(80, 640.0/480.0, 0.1, 100.0);
      glUniformMatrix4fv(projectionMatrixLocation, 1, false, &persp.values[0]);
      {
        int square = 4;
        glBindVertexArray(cubeVAO);
        for (int i = 0; i < square*square; i++) {
          if (tetronimo[i] == 0) continue;
          mat4 translate = mat4_translate((i%square)-(square/2)+0.5, (i/square)-(square/2)+0.5, 0);
          mat4 mat = mat4_multiply(&cubeModelMatrix, &translate);
          glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);

          glDrawArrays(GL_TRIANGLES, 0, cubeBufElems/3);
        }
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
  flatShader = loadShaderFromFile("assets/shaders/flat.shader");

  glUseProgram(shader.program);

  {
    mat4 scale = mat4_scale(1.0, 1.0, 1.0);
    mat4 translate = mat4_translate(0, 0, 0);
    cubeModelMatrix = mat4_multiply(&translate, &scale);
  }

  {
    int buffer;
    glGenVertexArrays(1, (GLuint*)(&triangleVAO));
    glBindVertexArray(triangleVAO);
    glGenBuffers(1, (GLuint*)(&buffer));
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, triangleBufElems*sizeof(float), triangleVertexBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
  }

  {
    int buffer;
    glGenVertexArrays(1, (GLuint*)(&cubeVAO));
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, (GLuint*)(&buffer));
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, cubeBufElems*sizeof(float), cubeVertexBuf, GL_STATIC_DRAW);
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
