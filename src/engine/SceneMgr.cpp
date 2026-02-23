#include "SceneMgr.hpp"
#include "../type/Scene.hpp"

void SceneMgr::changeScene(const std::string& name) {
  if (!scene_assets.count(name)) {
    SDL_Log("[E] <SceneMgr - changeScene> Can't change scene to '%s': not found",
            name.c_str());
    return;
  }
  Scene* scene = scene_assets[name];
  if (current_scene) current_scene->exit();
  current_scene = scene;
  current_scene->enter();
}
Scene* SceneMgr::getCurrentScene() { return current_scene; }
SceneMgr::SceneMgr(Scene* initial_scene) {
  scene_assets["init"] = initial_scene;
  changeScene("init");
}