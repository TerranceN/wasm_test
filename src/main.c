#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>

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

float i_tetronimo[] = {
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
};

float t_tetronimo[] = {
  0, 1, 0, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
};

float o_tetronimo[] = {
  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
};

float l_tetronimo[] = {
  0, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  0, 0, 0, 0,
};

float j_tetronimo[] = {
  0, 0, 0, 0,
  1, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0,
};

float z_tetronimo[] = {
  0, 1, 0, 0,
  1, 1, 0, 0,
  1, 0, 0, 0,
  0, 0, 0, 0,
};

float s_tetronimo[] = {
  1, 0, 0, 0,
  1, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
};

float* tetronimos[] = {
  i_tetronimo,
  t_tetronimo,
  o_tetronimo,
  l_tetronimo,
  j_tetronimo,
  z_tetronimo,
  s_tetronimo,
};

int tetronimo_sizes[] = {
  4,
  3,
  2,
  3,
  3,
  3,
  3,
};

typedef struct {
  float tetronimo[16];
  int size;
  float angle;
  float targetAngle;
  float actualX;
  float actualY;
  int x;
  int y;
} TetronimoRep;

typedef struct {
  float r;
  float g;
  float b;
} Color;

Color red   = {1, 0, 0};
Color green = {0, 1, 0};
Color blue  = {0, 0, 1};

Color *board[200]; // 20x10

TetronimoRep activeTet;
TetronimoRep ghostTet;

TetronimoRep tet_rep;
TetronimoRep tet_rep2;
TetronimoRep tet_rep3;
TetronimoRep tet_rep4;

void load_random_tetronimo(TetronimoRep *tet) {
  int i = rand() % 7;
  memcpy(tet->tetronimo, tetronimos[i], 16*sizeof(float));
  tet->size = tetronimo_sizes[i];
}

float constrainAbs(float val, float low, float high) {
  float inputVal = val;
  int sign = val < 0 ? -1 : 1;
  val = fmaxf(fabs(val), low);
  val = fminf(val, fabs(high));
  return sign * val;
}

void update_tet(TetronimoRep *tet) {
  float diffAngle = tet->targetAngle - tet->angle;
  tet->angle += constrainAbs(diffAngle/3, 0.05, diffAngle);

  float diffX = tet->x - tet->actualX;
  tet->actualX += constrainAbs(diffX/3, 0.05, diffX);

  float diffY = tet->y - tet->actualY;
  tet->actualY += constrainAbs(diffY/3, 0.05, diffY);
}

void rotate_tet_rep(TetronimoRep *tet) {
  mat4_transposen_in_place((mat4*)(&tet->tetronimo), tet->size);
  mat4_flip_vertn_in_place((mat4*)(&tet->tetronimo), tet->size);
}

void draw_tet_rep(TetronimoRep *tet, int modelMatrixLocation) {
  int size = tet->size;
  float xOffset = tet->x;
  float yOffset = tet->y;
  xOffset -= size/2.0;
  yOffset -= size/2.0;
  for (int i = 0; i < 16; i++) {
    if (tet->tetronimo[i] == 0) continue;
    mat4 translate = mat4_translate((i/4)+0.5-(size/2.0), (3-(i%4))+0.5-(4-size)-(size/2.0), 0);
    mat4 tetRot = mat4_rotateZ(tet->angle);
    mat4 mat = mat4_multiply(&tetRot, &translate);
    //translate = mat4_translate(size/2.0+xOffset, size/2.0+yOffset, 0.25);
    translate = mat4_translate(size/2.0-0.5+tet->actualX, size/2.0-0.5+tet->actualY, 0.25);
    mat = mat4_multiply(&translate, &mat);
    glUniformMatrix4fv(modelMatrixLocation, 1, false, &mat.values[0]);
    glDrawArrays(GL_TRIANGLES, 0, cubeBufElems/3);
  }
}

void place_tet(TetronimoRep* tet) {
  for (int i = 0; i < tet->size; i++) {
    for (int j = 0; j < tet->size; j++) {
      if (tet->tetronimo[i*4+(tet->size-1-j)]) {
        int boardIndex = (tet->y+j)*10 + (tet->x+i);
        if (boardIndex >= 0 && boardIndex < 200) {
          board[boardIndex] = &red;
        }
      }
    }
  }
}

bool check_board_collision(TetronimoRep *tet) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < tet->size; j++) {
      int tetIndex = i*4+(tet->size-1-j);
      if (tet->tetronimo[tetIndex] &&
          (tet->x+i < 0 || tet->x+i >= 10 || tet->y+j < 0)) {
        return true;
      }
      int boardIndex = (tet->y+j)*10 + (tet->x+i);
      if (boardIndex >= 0 && boardIndex < 200) {
        if (board[boardIndex]) {
          if (tet->tetronimo[tetIndex]) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

void update_ghost_tet() {
  ghostTet = activeTet;
  bool placed = false;
  while (!placed) {
    ghostTet.y -= 1;
    if (check_board_collision(&ghostTet)) {
      ghostTet.y += 1;
      ghostTet.actualX = ghostTet.x;
      ghostTet.actualY = ghostTet.y;
      ghostTet.angle = ghostTet.targetAngle;
      placed = true;
    }
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
        rotate_tet_rep(&activeTet);
        if (check_board_collision(&activeTet)) {
          rotate_tet_rep(&activeTet);
          rotate_tet_rep(&activeTet);
          rotate_tet_rep(&activeTet);
        } else {
          activeTet.angle -= PI/2;
        }
      }

      if (keyEvent->type == SDL_KEYDOWN && keyEvent->keysym.sym == SDLK_DOWN) {
        bool placed = false;
        while (!placed) {
          activeTet.y -= 1;
          if (check_board_collision(&activeTet)) {
            activeTet.y += 1;
            place_tet(&activeTet);
            load_random_tetronimo(&activeTet);
            activeTet.x = 5-activeTet.size/2;
            activeTet.y = 22-activeTet.size;
            activeTet.actualX = activeTet.x;
            activeTet.actualY = activeTet.y;
            placed = true;
          }
        }
      }

      if (keyEvent->type == SDL_KEYDOWN && keyEvent->keysym.sym == SDLK_LEFT) {
        activeTet.x -= 1;
        if (check_board_collision(&activeTet)) {
          activeTet.x += 1;
        }
      }

      if (keyEvent->type == SDL_KEYDOWN && keyEvent->keysym.sym == SDLK_RIGHT) {
        activeTet.x += 1;
        if (check_board_collision(&activeTet)) {
          activeTet.x -= 1;
        }
      }

      update_ghost_tet();
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
      mat4 mat = mat4_translate(-5, -10, -17);
      glUniformMatrix4fv(viewMatrixLocation, 1, false, &mat.values[0]);
    }
    {
      mat4 persp = mat4_perspective(PI/180 * 70, 640.0/480.0, 0.1, 100.0);
      glUniformMatrix4fv(projectionMatrixLocation, 1, false, &persp.values[0]);
    }

    {
      glBindVertexArray(cubeVAO);

      {
        update_tet(&activeTet);

        glUniform3f(colorLocation, 1, 1, 1);
        draw_tet_rep(&activeTet, modelMatrixLocation);
      }

      {
        if (ghostTet.y != activeTet.y) {
          glUniform3f(colorLocation, 0.5, 0.5, 0.5);
          draw_tet_rep(&ghostTet, modelMatrixLocation);
        }
      }

      {
        for (int i = 0; i < 20; i++) {
          for (int j = 0; j < 10; j++) {
            if (board[i*10+j]) {
              Color *color = board[i*10+j];
              glUniform3f(colorLocation, color->r, color->g, color->b);
            } else {
              glUniform3f(colorLocation, 0.2, 0.2, 0.2);
            }
            mat4 mat = mat4_translate(j, i, 0);
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
  for (int i = 0; i < 7; i++) {
    mat4_transpose_in_place((mat4*)tetronimos[i]);
  }

  for (int i = 0; i < 200; i++) {
    board[i] = NULL;
  }

  memcpy(activeTet.tetronimo, t_tetronimo, 16*sizeof(float));
  activeTet.size = 3;
  activeTet.angle = 0;
  activeTet.targetAngle = 0;
  activeTet.x = 5 - activeTet.size/2;
  activeTet.y = 22 - activeTet.size;
  activeTet.actualX = activeTet.x;
  activeTet.actualY = activeTet.y;

  update_ghost_tet();

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
