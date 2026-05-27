#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstring>
#include <string>
#include <utility>

namespace sjtu {

class exception {
protected:
  const std::string variant = "";
  std::string detail = "";
  
  exception(std::string variant, std::string detail): variant(std::move(variant)), detail(std::move(detail)) {}
public:
  virtual ~exception() = default;
  exception() {}
  exception(const exception &ec): variant(ec.variant), detail(ec.detail) {}
  virtual std::string what() {
    return variant + " " + detail;
  }
};

class container_is_empty : public exception {
public:
  explicit container_is_empty(std::string detail = "")
  : exception("[container is empty]", std::move(detail)) {}
};

class runtime_error : public exception {
public:
  explicit runtime_error(std::string detail = "")
  : exception("[runtime error]", std::move(detail)) {}
};
}


#endif