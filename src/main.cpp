#include "engine/Game.hpp"
#include <SDL3/SDL_main.h>
#include <locale>

int main(int, char **)
{
    // 强制UTF-8，防止用GBK出现乱码
    std::locale::global(std::locale(".UTF-8"));
    Game *game = Game::get();
    
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

    if (game->getState() == SDL_APP_FAILURE)
    {
        SDL_Log("-- Game End With Error --");
        return -1;
    }
    SDL_Log("-- Game End Successfully --");
    return 0;
}