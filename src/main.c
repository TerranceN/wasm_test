#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "shader.h"
#include "math/mat4.h"
#include "vertex_data.h"

#define PI 3.14159

int triangleVAO;
int cubeVAO;

SDL_Window *window;
Shader shader;
Shader flatShader;
bool isRunning = true;

float triangleX;
float triangleY;
float triangleScale = 50;

mat4 cubeModelMatrix;

float tetronimo[] = {
  0, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  0, 0, 0, 0,
};

float tetronimo2[] = {
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
};

float tetronimo3[] = {
  0, 1, 0, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
};

float tetronimo4[] = {
  0, 1, 0, 0,
  1, 1, 0, 0,
  1, 0, 0, 0,
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
        mat4_transposen_in_place((mat4*)tetronimo, 3);
        mat4_flip_horizn_in_place((mat4*)tetronimo, 3);

        mat4_transposen_in_place((mat4*)tetronimo2, 4);
        mat4_flip_horizn_in_place((mat4*)tetronimo2, 4);

        mat4_transposen_in_place((mat4*)tetronimo3, 3);
        mat4_flip_horizn_in_place((mat4*)tetronimo3, 3);

        mat4_transposen_in_place((mat4*)tetronimo4, 3);
        mat4_flip_horizn_in_place((mat4*)tetronimo4, 3);
      }
    }

    if (event.type == SDL_MOUSEMOTION) {
      SDL_MouseMotionEvent *mouseEvent = (SDL_MouseMotionEvent*)(&event);

      triangleX = mouseEvent->x;
      triangleY = 480 - mouseEvent->y;
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(flatShader.program);
  {
    int modelMatrixLocation = glGetUniformLocation(flatShader.program, "u_modelMatrix");
    int viewMatrixLocation = glGetUniformLocation(flatShader.program, "u_viewMatrix");
    int projectionMatrixLocation = glGetUniformLocation(flatShader.program, "u_projectionMatrix");

    {
      mat4 mat = mat4_translate(0, -1.5, -8);
      glUniformMatrix4fv(viewMatrixLocation, 1, false, &mat.values[0]);
    }

    {
      mat4 rot;
      //rot = mat4_rotateY(0.03);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      //rot = mat4_rotateX(0.01);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      //rot = mat4_rotateZ(0.02);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      mat4 persp = mat4_perspective(PI/180 * 70, 640.0/480.0, 0.1, 100.0);
      glUniformMatrix4fv(projectionMatrixLocation, 1, false, &persp.values[0]);
      {
        int square = 4;
        glBindVertexArray(cubeVAO);
        for (int j = 0; j < 4; j++) {
          float xOffset = 3;
          float yOffset = -3;
          float* tet = tetronimo;
          if (j % 2 == 0) {
            xOffset = -xOffset;
          }
          if (j / 2 == 0) {
            yOffset = -yOffset;
          }
          if (j == 1) {
            tet = tetronimo2;
          }
          if (j == 2) {
            tet = tetronimo3;
          }
          if (j == 3) {
            tet = tetronimo4;
          }
          for (int i = 0; i < square*square; i++) {
            if (tet[i] == 0) continue;
            mat4 translate = mat4_translate((i/square)-(square/2)+0.5+xOffset, (square-(i%square))-(square/2)+0.5+yOffset, 0);
            mat4 mat = mat4_multiply(&cubeModelMatrix, &translate);
            glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);

            glDrawArrays(GL_TRIANGLES, 0, cubeBufElems/3);
          }
        }
      }
    }
  }
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

  SDL_GL_SwapWindow(window);
}

#ifdef __EMSCRIPTEN__
void emscripten_set_main_loop(void*, int, int);
#endif

int main() {
  mat4_transpose_in_place((mat4*)tetronimo);
  mat4_transpose_in_place((mat4*)tetronimo2);
  mat4_transpose_in_place((mat4*)tetronimo3);
  mat4_transpose_in_place((mat4*)tetronimo4);

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
