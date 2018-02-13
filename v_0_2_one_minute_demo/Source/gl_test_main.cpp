/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Testing OpenGL version.
*/

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <OpenGL/gl.h>

// The window
SDL_Window* window;

// The OpenGL context
SDL_GLContext context;

GLuint* texture;

bool initialize() {

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  
  // Create window
  window = SDL_CreateWindow("Teddy Wickets",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    1440, 900,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("Window could not be created. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Create context
  context = SDL_GL_CreateContext(window);
  if (context == NULL) {
    printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Use Vsync
  if (SDL_GL_SetSwapInterval(1) < 0) {
    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Set up texture
  std::string path = "Art/title_screen_draft_5.png";

  // Use the SDL to load the image
  SDL_Surface* image = IMG_Load(path.c_str());
  if (image == nullptr) {
      printf("IMG_Load failed for %s with error: %s\n", path.c_str(), IMG_GetError());
  }

  texture = new GLuint[1];
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1440, 900, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->pixels);
}

void shutdown() {
  // Destroy window
  SDL_DestroyWindow(window);
  window = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}

int main(int argc, char* args[]) {
  initialize();

  // Enable text input
  SDL_StartTextInput();

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_TEXTURE_2D);

  // Simple viewport.
  glViewport(0, 0, 1440, 900);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config! %d\n", error);
    exit(1);
  }

  // Debug info for now
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  printf("OpenGL Shading Language Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  bool quit = false;
  while (!quit) {
    
    // Event handler
    SDL_Event e;

    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        quit = true;
      }
    }

    glClearColor(0.97, 0.97, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, 1440, 900, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, (GLuint) *texture);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(0, 0);
    glTexCoord2d(0.0, 1.0); glVertex2d(0, 900);
    glTexCoord2d(1.0, 1.0); glVertex2d(1440, 900);
    glTexCoord2d(1.0, 0.0); glVertex2d(1440, 0);
    glEnd();

    // update();

    // render();

    // Update screen
    SDL_GL_SwapWindow(window);
    //screen->loop(window, sound_system);

  }

  // Disable text input
  SDL_StopTextInput();

  shutdown();
}
