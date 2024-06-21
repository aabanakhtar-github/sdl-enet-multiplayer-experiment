#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

#define MAKE_SINGLETON(classname)                                              \
public:                                                                        \
  static classname &get() {                                                    \
    static classname s_instance;                                               \
    return s_instance;                                                         \
  }                                                                            \
                                                                               \
private:                                                                       \
  classname &operator=(const classname &) = delete;                            \
  classname(const classname &) = delete;

enum class AppState { AS_FAIL, AS_QUIT, AS_LOOP };

// courtesy stack overflow
template <class T> inline void hash_combine(std::size_t &seed, const T &v) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

class GlobalAppState {
  MAKE_SINGLETON(GlobalAppState)

  GlobalAppState() : app_state_(AppState::AS_LOOP) {}

public:
  void setAppState(AppState state, const std::string &error = "");
  [[nodiscard]] AppState getAppState() const { return app_state_; }
  [[nodiscard]] const std::vector<std::string> &getError() const {
    return error_;
  }

private:
  AppState app_state_;
  std::vector<std::string> error_;
};

void initLibraries();
void shutdownLibraries();

#endif // UTIL_H
