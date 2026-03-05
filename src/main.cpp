#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

#include "game.hpp"
#include "gamecontext.hpp"

int main(int, char** argv) {
  try {
    GameContext context;
    Game game;
    game.run();
  } catch (std::exception& e) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
    return 1;
  }
  return 0;
}