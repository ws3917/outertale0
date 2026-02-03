// SDL, miniaudio的相关C-type类型，C++封装
#include <SDL3/SDL.h>
#include <memory>
#include <miniaudio/miniaudio.h>

namespace E_Types
{
struct SDLDeleter
{
    void operator()(SDL_Window *w) const
    {
        if (w)
            SDL_DestroyWindow(w);
    }
    void operator()(SDL_Renderer *r) const
    {
        if (r)
            SDL_DestroyRenderer(r);
    }
    void operator()(SDL_Surface *s) const
    {
        if (s)
            SDL_DestroySurface(s);
    }
    void operator()(SDL_Texture *t) const
    {
        if (t)
            SDL_DestroyTexture(t);
    }
};

// SDL window, renderer, texture, surface
using WindowPtr = std::unique_ptr<SDL_Window, SDLDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SDLDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SDLDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SDLDeleter>;

// miniaudio deleter，因为uninit/init对象本身也要考虑释放和创建
struct MaDeleter
{
    void operator()(ma_sound *ptr) const
    {
        if (ptr)
        {
            ma_sound_uninit(ptr);
            delete ptr;
        }
    }
    void operator()(ma_engine *ptr) const
    {
        if (ptr)
        {
            ma_engine_uninit(ptr);
            delete ptr;
        }
    }
};

// miniaudio
using SoundPtr = std::unique_ptr<ma_sound, MaDeleter>;
using SoundEnginePtr = std::unique_ptr<ma_engine, MaDeleter>;

} // namespace E_Types