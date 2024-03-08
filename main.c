#include <stdio.h>

#include "chip8.h"
 // gcc -std=c17 notmain.c -I SDL2\include -L SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -o main
#include<windows.h>


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
SDL_Window * screen = NULL;

SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL;
SDL_Surface * bugger = NULL;

//SDL_Event event;

void setupGraphics() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return;
  }

  // Create a window
  screen = SDL_CreateWindow("EmuC8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (screen == NULL) {
    printf("Window creation failed: %s\n", SDL_GetError());
    return;
  }

  // Create a renderer
  renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("Renderer creation failed: %s\n", SDL_GetError());
    return;
  } else {
    int modif = SCREEN_WIDTH / 64;
    SDL_RenderSetScale(renderer,
      modif,
      modif);
  }

  texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGB888,
    SDL_TEXTUREACCESS_STREAMING, 64, 32);

  // Set render draw color (optional)
  SDL_SetRenderDrawColor(renderer, 122, 30, 0, 255);
  SDL_RenderPresent(renderer);

}

/* Cleans up after game over */
void FinishOff() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(screen);
  //Quit SDL
  SDL_Quit();
  exit(0);
}
void setKeys(Chip8 * emu, int * running);

void gameloop(Chip8 * emu) {
  int * gameloop = malloc(sizeof(int * ));
  * gameloop = 1;
  // Initialize the Chip8 system and load the game into the memory  
  initialize(emu);

  int i = 0;
  loadGame(emu, "./outlaw.ch8");

  // Emulation loop
  while (gameloop) {
    // Emulate one cycle
    emulateCycle(emu);
    (i++) % 8;
    // If the draw flag is set, update the screen
    if (emu -> draw == true) {
      drawPixels(emu);

      render();

      emu -> draw = false;

    }
    // if(i >100) {break;}

    // Store key press state (Press and Release)
    setKeys( & emu, gameloop);
    // Delay to reduce loop speed
    SDL_Delay(2);
    if (i == 8) {
      sleep(1);
    };

  }

  return 0;
}

void drawPixels(Chip8 * emu) {

  // Lock the texture for direct access to pixels
  // Number of bytes in a row of pixel data

  unsigned short gfxwidth = 64;
  unsigned short gfxheight = 32;

  uint32_t * pixels = NULL;
  int pitch = gfxwidth * 4; // Pitch = 256 bytes (64 pixels * 4 bytes per pixel)

  SDL_LockTexture(texture, NULL, (void ** ) & pixels, & pitch);

  int i;
  for (i = 0; i < 2048; i++) {
    pixels[i] = emu -> gfx[i] * 255;

  }

  SDL_UnlockTexture(texture);
 
}
void render() {
  SDL_RenderClear(renderer);

  SDL_RenderCopy(renderer, texture, NULL, NULL);

  SDL_RenderPresent(renderer);

}

int main(int argc, char * args[]) {
  Chip8 myNES;
  setupGraphics();

  gameloop( & myNES);

  FinishOff();
  //return 0;
}

void setKeys(Chip8 * emu, int * running) {
  SDL_Event event;

  while (SDL_PollEvent( & event)) {

    // Poll for events
    if (event.type == SDL_KEYDOWN) {

      printf(" KEYDOWN/n \n");

      switch (event.key.keysym.sym) {
      case SDL_SCANCODE_1:

        emu -> Vreg[0x1] = 1;
        printf(" SDL_SCANCODE_1 /n \n");
        break;
      case SDL_SCANCODE_2:

        emu -> Vreg[0x2] = 1;
        printf(" SDL_SCANCODE_2 /n \n");

        break;
      case SDL_SCANCODE_3:

        emu -> Vreg[0x3] = 1;
        printf(" SDL_SCANCODE_3 /n \n");

        break;
      case SDL_SCANCODE_4:
        printf(" SDL_SCANCODE_4 /n \n");

        emu -> Vreg[0xC] = 1;
        break;
      case SDL_SCANCODE_A:
        printf(" SDL_SCANCODE_A/n \n");

        emu -> Vreg[0x4] = 1;
        break;
      case SDL_SCANCODE_Z:
        emu -> Vreg[0x5] = 1;
        printf(" SDL_SCANCODE_Z/n \n");

        break;
      case SDL_SCANCODE_E:
        emu -> Vreg[0x6] = 1;
        printf(" SDL_SCANCODE_E/n \n");

        break;
      case SDL_SCANCODE_Q:
        emu -> Vreg[0x7] = 1;
        break;
      case SDL_SCANCODE_S:
        emu -> Vreg[0x8] = 1;
        break;
      case SDL_SCANCODE_D:
        emu -> Vreg[0x9] = 1;
        printf(" SDL_SCANCODE_D/n \n");

        break;
      case SDL_SCANCODE_R:
        emu -> Vreg[0xD] = 1;
        break;
      case SDL_SCANCODE_F:
        emu -> Vreg[0xE] = 1;
        break;
      case SDL_SCANCODE_W:
        emu -> Vreg[0xA] = 1;
        break;
      case SDL_SCANCODE_X:
        emu -> Vreg[0x0] = 1;
        break;
      case SDL_SCANCODE_C:
        emu -> Vreg[0xB] = 1;
        break;
      case SDL_SCANCODE_V:
        emu -> Vreg[0xF] = 1;
        break;
      case SDL_SCANCODE_P:
        running = 0;
        break;
      default:
        break;
      }
    }
    if (event.type == SDL_KEYUP) {
      switch (event.key.keysym.scancode) {

      case SDL_SCANCODE_1:
        emu -> Vreg[0x1] = 0;
        printf(" SDL_SCANCODE_11/n \n");

        break;
      case SDL_SCANCODE_2:
        printf(" SDL_SCANCODE_22/n \n");

        emu -> Vreg[0x2] = 0;
        break;
      case SDL_SCANCODE_3:

        emu -> Vreg[0x3] = 0;
        break;
      case SDL_SCANCODE_4:

        emu -> Vreg[0xC] = 0;
        break;
      case SDL_SCANCODE_A:
        printf(" SDL_SCANCODE_AA/n \n");

        emu -> Vreg[0x4] = 0;
        break;
      case SDL_SCANCODE_Z:
        emu -> Vreg[0x5] = 0;
        break;
      case SDL_SCANCODE_E:
        emu -> Vreg[0x6] = 0;
        break;
      case SDL_SCANCODE_Q:
        emu -> Vreg[0x7] = 0;
        break;
      case SDL_SCANCODE_S:
        emu -> Vreg[0x8] = 0;
        break;
      case SDL_SCANCODE_D:
        emu -> Vreg[0x9] = 0;
        break;
      case SDL_SCANCODE_R:
        emu -> Vreg[0xD] = 0;
        break;
      case SDL_SCANCODE_F:
        emu -> Vreg[0xE] = 0;
        break;
      case SDL_SCANCODE_W:
        emu -> Vreg[0xA] = 0;
        break;
      case SDL_SCANCODE_X:
        emu -> Vreg[0x0] = 0;
        break;
      case SDL_SCANCODE_C:
        emu -> Vreg[0xB] = 0;
        break;
      case SDL_SCANCODE_V:
        emu -> Vreg[0xF] = 0;
        break;
      default:
        break;
      }
      break;
    }

  }

}
