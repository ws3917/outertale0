#pragma once

#include <optional>
#include <string>

#include "object.hpp"


class yyjson_doc;
class Json : public Object {
 public:
  bool load(const std::string& filepath) override;
  void reset() override;
  template <typename T>
  std::optional<T> get(const std::string& key);

 private:
  struct yyjson_docDeleter {
    void operator()(yyjson_doc* doc) const;
  };
  std::unique_ptr<yyjson_doc, yyjson_docDeleter> doc;
};