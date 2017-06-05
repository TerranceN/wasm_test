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

typedef struct {
  float* base_tetronimo;
  int size;
  float angle;
  float targetAngle;
} TetronimoRep;

TetronimoRep tet_rep;
TetronimoRep tet_rep2;
TetronimoRep tet_rep3;
TetronimoRep tet_rep4;

void rotate_tet_rep(TetronimoRep *rep) {
  rep->angle -= PI/2;
  mat4_transposen_in_place((mat4*)rep->base_tetronimo, rep->size);
  mat4_flip_vertn_in_place((mat4*)rep->base_tetronimo, rep->size);
}

void draw_tet_rep(TetronimoRep *rep, int modelMatrixLocation, float xOffset, float yOffset) {
  int size = rep->size;
  for (int i = 0; i < 16; i++) {
    if (rep->base_tetronimo[i] == 0) continue;
    mat4 translate = mat4_translate((i/4)+0.5-(size/2.0), (3-(i%4))+0.5-(4-size)-(size/2.0), 0);
    mat4 tetRot = mat4_rotateZ(rep->angle);
    mat4 mat = mat4_multiply(&tetRot, &translate);
    translate = mat4_translate(size/2.0+xOffset, size/2.0+yOffset, 0);
    mat = mat4_multiply(&translate, &mat);
    glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);
    glDrawArrays(GL_TRIANGLES, 0, cubeBufElems/3);
  }
}

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

      if (keyEvent->type == SDL_KEYDOWN && keyEvent->keysym.sym == SDLK_UP) {
        rotate_tet_rep(&tet_rep);
        rotate_tet_rep(&tet_rep2);
        rotate_tet_rep(&tet_rep3);
        rotate_tet_rep(&tet_rep4);
      }

      if (keyEvent->type == SDL_KEYDOWN && keyEvent->keysym.sym == SDLK_r) {
        mat4_transposen_in_place((mat4*)tetronimo, 3);
        mat4_flip_vertn_in_place((mat4*)tetronimo, 3);

        mat4_transposen_in_place((mat4*)tetronimo2, 4);
        mat4_flip_vertn_in_place((mat4*)tetronimo2, 4);

        mat4_transposen_in_place((mat4*)tetronimo3, 3);
        mat4_flip_vertn_in_place((mat4*)tetronimo3, 3);

        mat4_transposen_in_place((mat4*)tetronimo4, 3);
        mat4_flip_vertn_in_place((mat4*)tetronimo4, 3);
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
    int colorLocation = glGetUniformLocation(flatShader.program, "u_color");

    {
      mat4 mat = mat4_translate(-1.5, -2.0, -8);
      glUniformMatrix4fv(viewMatrixLocation, 1, false, &mat.values[0]);
    }

    {
      glBindVertexArray(cubeVAO);

      mat4 rot;
      //rot = mat4_rotateY(0.03);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      //rot = mat4_rotateX(0.01);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      //rot = mat4_rotateZ(0.02);
      //cubeModelMatrix = mat4_multiply(&cubeModelMatrix, &rot);
      mat4 persp = mat4_perspective(PI/180 * 70, 640.0/480.0, 0.1, 100.0);
      glUniformMatrix4fv(projectionMatrixLocation, 1, false, &persp.values[0]);
      for (int j = 0; j < 4; j++) {
        float xOffset = 3;
        float yOffset = -3;
        float r = 1.0;
        float g = 0.0;
        float b = 0.0;
        TetronimoRep* tet = &tet_rep;
        if (j % 2 == 0) {
          xOffset = -xOffset;
        }
        if (j / 2 == 0) {
          yOffset = -yOffset;
        }
        if (j == 1) {
          tet = &tet_rep2;
          r = 0;
          g = 1;
        }
        if (j == 2) {
          tet = &tet_rep3;
          r = 0;
          b = 1;
        }
        if (j == 3) {
          tet = &tet_rep4;
          r = 0;
          g = 1;
          b = 1;
        }
        float diffAngle = tet->targetAngle - tet->angle;
        tet->angle += diffAngle/4;

        glUniform3f(colorLocation, r, g, b);

        draw_tet_rep(tet, modelMatrixLocation, xOffset, yOffset);
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

  tet_rep.base_tetronimo = tetronimo;
  tet_rep.size = 3;
  tet_rep.angle = 0;
  tet_rep.targetAngle = 0;

  tet_rep2.base_tetronimo = tetronimo2;
  tet_rep2.size = 4;
  tet_rep2.angle = 0;
  tet_rep2.targetAngle = 0;

  tet_rep3.base_tetronimo = tetronimo3;
  tet_rep3.size = 3;
  tet_rep3.angle = 0;
  tet_rep3.targetAngle = 0;

  tet_rep4.base_tetronimo = tetronimo4;
  tet_rep4.size = 3;
  tet_rep4.angle = 0;
  tet_rep4.targetAngle = 0;


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
