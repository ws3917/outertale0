#pragma once
// 这里是主线程循环
#include "E_Scene.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
class E_Game
{
  public:
    // 顶层接口
    void init();
    void input(SDL_Event *event);
    void update(float delta);
    void draw();
    void exit();

    // error = true 代表异常终止，反之为异常终止
    void abort(bool error)
    {
        state = error ? SDL_APP_FAILURE : SDL_APP_SUCCESS;
    };
    SDL_AppResult getState()
    {
        return state;
    }

    // 单例
    E_Game(const E_Game &) = delete;
    E_Game &operator=(const E_Game &) = delete;
    static E_Game *get()
    {
        static E_Game instance = E_Game();
        return &instance;
    }

  private:
    E_Game() = default;
    ~E_Game()
    {
        exit();
    }
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    E_Scene *current_scene = nullptr;
    SDL_AppResult state = SDL_APP_CONTINUE;
};