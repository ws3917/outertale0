#include "json.hpp"

#include <SDL3/SDL_log.h>
#include <yyjson.h>

void Json::yyjson_docDeleter::operator()(yyjson_doc* doc) const {
  if (doc) yyjson_doc_free(doc);
}
void Json::reset() {
  Object::reset();
  doc.reset();
}
bool Json::load(const std::string& filepath) {
  if (loaded) reset();
  if (!Object::load(filepath)) return false;
  loaded = false;
  yyjson_read_err err;
  yyjson_doc* new_doc = yyjson_read_opts(reinterpret_cast<char*>(data.data()),
                                         data.size(), 0, nullptr, &err);

  if (!doc) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", err.msg);
    return false;
  }
  doc.reset(new_doc);
  loaded = true;
  return true;
}

template <typename T>
std::optional<T> Json::get(const std::string& key) {
  if (!loaded || key.empty()) return std::nullopt;
  yyjson_val* val = yyjson_doc_ptr_get(doc.get(), key.c_str());
  if (!val) return std::nullopt;

  if constexpr (std::is_same_v<T, int>) {
    return yyjson_get_int(val);
  } else if constexpr (std::is_same_v<T, double>) {
    return yyjson_get_real(val);
  } else if constexpr (std::is_same_v<T, float>) {
    return static_cast<float>(yyjson_get_real(val));
  } else if constexpr (std::is_same_v<T, std::string>) {
    return std::string(yyjson_get_str(val));
  } else if constexpr (std::is_same_v<T, bool>) {
    return yyjson_get_bool(val);
  }
  return std::nullopt;
}

template std::optional<int> Json::get<int>(const std::string& key);
template std::optional<double> Json::get<double>(const std::string& key);
template std::optional<float> Json::get<float>(const std::string& key);
template std::optional<std::string> Json::get<std::string>(
    const std::string& key);
template std::optional<bool> Json::get<bool>(const std::string& key);