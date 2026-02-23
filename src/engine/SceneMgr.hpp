#pragma once
#include <string>
#include <unordered_map>

class Scene;
class SceneMgr {
 public:
  void changeScene(const std::string& name);
  Scene* getCurrentScene();
  SceneMgr(Scene* initial_scene);

 private:
  std::unordered_map<std::string, Scene*> scene_assets = {};
  Scene* current_scene = nullptr;
};