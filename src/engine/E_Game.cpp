#include "E_Game.hpp"
#include "../scene/S_Init.h"
void E_Game::init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("[C] <Init> SDL init failed: %s", SDL_GetError());
        abort(true);
    }

    // 创建窗口
    constexpr int WIDTH = 960, HEIGHT = 720;
    if (!SDL_CreateWindowAndRenderer("speedtest", WIDTH, HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE,
                                     &window, &renderer))
    {
        SDL_Log("[C] <Init> Can't create window / renderer: %s", SDL_GetError());
        abort(true);
    }

    // 垂直同步
    SDL_SetRenderVSync(renderer, 1);

    // 高分辨率
    SDL_SetDefaultTextureScaleMode(renderer, SDL_SCALEMODE_LINEAR);
    float scale = SDL_GetWindowDisplayScale(window);
    SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetWindowSize(window, static_cast<int>(WIDTH * scale), static_cast<int>(HEIGHT * scale));

    current_scene = new S_Init();
    if (!current_scene)
    {
        SDL_Log("[C] <Init> Can't enter initial scene.");
        abort(true);
    }
}
void E_Game::input(SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        abort(false);

    SDL_ConvertEventToRenderCoordinates(renderer, event);
}
void E_Game::draw()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    current_scene->draw(renderer);
    SDL_RenderPresent(renderer);
}
void E_Game::update(float delta)
{
    current_scene->update(delta);
}
void E_Game::exit()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}