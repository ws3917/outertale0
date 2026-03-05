#pragma once

#include <memory>
#include <string>
#include <vector>
#include <SDL3/SDL_iostream.h>


class Object {
 private:
  struct SDL_IOCloseDeleter {
    void operator()(SDL_IOStream* io) const;
  };

 public:
  Object() = default;
  virtual ~Object() = default;
  Object(const Object&) = delete;
  Object& operator=(const Object&) = delete;
  Object(Object&&) = default;
  Object& operator=(Object&&) = default;
  virtual bool load(const std::string& filepath);
  virtual void reset();

 protected:
  std::unique_ptr<SDL_IOStream, SDL_IOCloseDeleter> io;
  std::vector<uint8_t> data = {};
  bool loaded = false;
};