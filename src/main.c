#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char* vertShaderSrc = " \
attribute vec3 a_Position; \
void main() { \
  gl_Position = vec4(a_Position, 1.0); \
} \
";

const char* fragShaderSrc = " \
precision mediump float; \
uniform vec2 color; \
void main() { \
  gl_FragColor = vec4(1.0, color, 1.0); \
} \
";

const float vertexBuf[] = {
	-0.3f,  0.5f, -1.0f,
	-0.8f, -0.5f, -1.0f,
	 0.2f, -0.5f, -1.0f

	//1.0f,  0.0f,  0.0f,
	//0.0f,  1.0f,  0.0f,
	//0.0f,  0.0f,  1.0f
};

typedef struct {
  int program;
} Shader;

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
    }
  }

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shader.program);
  glUniform2f(colorPos, green, blue);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  SDL_GL_SwapWindow(window);
}

#ifdef __EMSCRIPTEN__
emscripten_set_main_loop(void*, int, int);
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

  shader = makeShader(vertShaderSrc, fragShaderSrc);

  glUseProgram(shader.program);

  int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), vertexBuf, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  colorPos = glGetUniformLocation(shader.program, "color");

#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
  emscripten_set_main_loop(one_iter, 60, 1);
#else
  while (isRunning) {
    one_iter();
    SDL_Delay(33);
  }
#endif

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
