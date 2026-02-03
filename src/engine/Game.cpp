#include "Game.hpp"
#include "../scene/S_Init.hpp"
#include "AudioMgr.hpp"
#include "SDL3/SDL_render.h"

void Game::init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("[C] <Init> SDL init failed: %s", SDL_GetError());
        abort(true);
    }

    // 创建窗口
    constexpr int WIDTH = 960, HEIGHT = 720;
    window.reset(
        SDL_CreateWindow("const char *title", WIDTH, HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE));
    renderer.reset(SDL_CreateRenderer(window.get(), nullptr));
    if (!window || !renderer)
    {
        SDL_Log("[C] <Init> Can't create window / renderer: %s", SDL_GetError());
        abort(true);
    }

    // 垂直同步
    SDL_SetRenderVSync(renderer.get(), 1);

    // 高分辨率
    SDL_SetDefaultTextureScaleMode(renderer.get(), SDL_SCALEMODE_LINEAR);
    float scale = SDL_GetWindowDisplayScale(window.get());
    SDL_SetRenderLogicalPresentation(renderer.get(), WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetWindowSize(window.get(), static_cast<int>(WIDTH * scale), static_cast<int>(HEIGHT * scale));

    // 音频
    if (!ImageMgr::get()->init())
    {
        abort(true);
    }

    // 初始场景
    current_scene = std::make_unique<S_Init>();
    if (!current_scene)
    {
        SDL_Log("[C] <Init> Can't enter initial scene.");
        abort(true);
    }
    current_scene->enter();
}
void Game::input(SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        abort(false);

    SDL_ConvertEventToRenderCoordinates(renderer.get(), event);
}
void Game::draw()
{
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer.get());
    current_scene->draw(renderer.get());
    SDL_RenderPresent(renderer.get());
}
void Game::update(float delta)
{
    current_scene->update(delta);
}
void Game::abort(bool error)
{
    state = error ? SDL_APP_FAILURE : SDL_APP_SUCCESS;
};
SDL_AppResult Game::getState()
{
    return state;
}