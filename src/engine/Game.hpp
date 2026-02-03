#pragma once
// 这里是主线程循环
#include "E_Types.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include "E_Scene.hpp"
class Game
{
  public:
    // 顶层接口
    void init();
    void input(SDL_Event *event);
    void update(float delta);
    void draw();
    void exit();

    // error = true 代表异常终止，反之为异常终止
    void abort(bool error);
    SDL_AppResult getState();

    // 单例
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    static Game *get()
    {
        static Game instance = Game();
        return &instance;
    }

  private:
    Game() = default;
    ~Game() = default;
    E_Types::WindowPtr window{};
    E_Types::RendererPtr renderer{};
    SDL_AppResult state = SDL_APP_CONTINUE;
    std::unique_ptr<E_Scene> current_scene = nullptr;
};