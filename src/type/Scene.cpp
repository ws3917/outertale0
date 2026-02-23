#include "Scene.hpp"

#include <algorithm>

void Scene::update(float delta) {
  for (auto& [name, obj] : objects)
    if (obj) obj->update(delta);
}
void Scene::input(SDL_Event* event) {
  for (auto& [name, obj] : objects)
    if (obj) obj->input(event);
}
void Scene::draw() {
  // render queue 用来临时排序
  render_queue.clear();
  render_queue.reserve(objects.size());
  for (auto& [name, obj] : objects) {
    if (obj) render_queue.push_back(obj);
  }
  // 渲染顺序 - 先按层级，再按y坐标
  std::sort(render_queue.begin(), render_queue.end(), [](Object* a, Object* b) {
    if (a->layer != b->layer) return a->layer < b->layer;
    return a->y < b->y;
  });
  for (auto* obj : render_queue) {
    obj->draw();
  }
}
Scene::~Scene() {
  for (auto& [name, obj] : objects)
    if (obj) delete obj;
  objects.clear();
}