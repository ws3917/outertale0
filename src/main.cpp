#include "engine/E_Game.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>

int main(int, char **)
{
    E_Game *game = E_Game::get();
    SDL_Log("-- Game Start --\n");
    game->init();
    SDL_Event event;
    uint64_t last_tick = 0;
    while (game->getState() == SDL_APP_CONTINUE)
    {
        while (SDL_PollEvent(&event))
            game->input(&event);

        uint64_t current_tick = SDL_GetTicksNS();
        if (last_tick == 0)
            last_tick = current_tick;
        float delta = (last_tick - current_tick) / 1e6f;
        last_tick = current_tick;
        game->update(delta);
        game->draw();
    }

    game->exit();
    if (game->getState() == SDL_APP_FAILURE)
    {
        SDL_Log("-- Game End With Error --");
        return -1;
    }
    SDL_Log("-- Game End Successfully --");
    return 0;
}