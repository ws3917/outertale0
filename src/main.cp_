// Deltarune CN Patcher - 三角符文汉化安装器 (SDL3重构)
// ws3917, 2026

#define SDL_MAIN_USE_CALLBACKS 1
#include "scene/SInit.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>

struct GameState
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    std::unique_ptr<Scene> current_scene = nullptr;

    uint64_t last_tick = 0;

    ~GameState()
    {
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);
    }
};

SDL_AppResult SDL_AppInit(void **appstate, int, char **)
{
    auto *game = new GameState();
    *appstate = game;

    SDL_Log("-- Game Start --");
    SDL_Log("[C] = Critical, [E] = Error, [W] = Warning, [I] = Info");
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("[C] <Init> SDL Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    constexpr int APP_WIDTH = 960, APP_HEIGHT = 720;
    if (!SDL_CreateWindowAndRenderer("Deltarune CN Patcher", APP_WIDTH, APP_HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY,
                                     &game->window, &game->renderer))
    {
        SDL_Log("[C] <Init> Can't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Surface *icon = SDL_LoadPNG("image/icon.png");
    if (icon)
    {
        SDL_SetWindowIcon(game->window, icon);
        SDL_DestroySurface(icon);
    }
    SDL_SetRenderVSync(game->renderer, 1);
    SDL_SetDefaultTextureScaleMode(game->renderer, SDL_SCALEMODE_PIXELART);
    SDL_SetRenderLogicalPresentation(game->renderer, APP_WIDTH, APP_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    float scale = SDL_GetWindowDisplayScale(game->window);
    SDL_SetWindowSize(game->window, static_cast<int>(scale * APP_WIDTH), static_cast<int>(scale * APP_HEIGHT));

    game->current_scene = std::make_unique<SInit>();
    if (!game->current_scene)
    {
        SDL_Log("[C] <Init> Can't enter initial scene: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    auto *game = static_cast<GameState *>(appstate);
    SDL_ConvertEventToRenderCoordinates(game->renderer, event);
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    game->current_scene->input(event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    auto *game = static_cast<GameState *>(appstate);
    const uint64_t now = SDL_GetTicks();
    if (game->last_tick == 0)
        game->last_tick = now;

    uint64_t delta = now - game->last_tick;
    game->last_tick = now;

    game->current_scene->update(delta);

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);
    game->current_scene->draw(game->renderer);

    SDL_RenderPresent(game->renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult)
{
    auto *game = static_cast<GameState *>(appstate);
    if (game)
    {
        delete game;
    }
    SDL_Log("-- Game End --");
}