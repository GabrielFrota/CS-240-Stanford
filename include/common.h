#ifndef CHLOROS_SRC_COMMON_H_
#define CHLOROS_SRC_COMMON_H_

#include <cstdarg>
#include <stdexcept>
#include <string>

#define COMMON_NAMESPACE ::chloros::common

#define LOG(fmt...)                                                            \
  COMMON_NAMESPACE::Log(COMMON_NAMESPACE::LogLevel::kInfo, __FILE__, __LINE__, \
                        __func__, fmt)

#define LOG_WARN(fmt...)                                                       \
  COMMON_NAMESPACE::Log(COMMON_NAMESPACE::LogLevel::kWarn, __FILE__, __LINE__, \
                        __func__, fmt)

#define LOG_FATAL(fmt...)                                             \
  COMMON_NAMESPACE::Log(COMMON_NAMESPACE::LogLevel::kFatal, __FILE__, \
                        __LINE__, __func__, fmt)

#ifdef DEBUG
#define LOG_DEBUG(fmt...)                                             \
  COMMON_NAMESPACE::Log(COMMON_NAMESPACE::LogLevel::kDebug, __FILE__, \
                        __LINE__, __func__, fmt)
#else  // DEBUG
#define LOG_DEBUG(fmt...) COMMON_NAMESPACE::Ignore(fmt)
#endif  // DEBUG

#define LIKELY(v) __builtin_expect(static_cast<bool>(v), 1)
#define UNLIKELY(v) __builtin_expect(static_cast<bool>(v), 0)

#define ASSERT(expr, msg...)                                            \
  {                                                                     \
    if (UNLIKELY(!(expr))) {                                            \
      COMMON_NAMESPACE::AssertFail(__FILE__, __LINE__, __func__, #expr, \
                                   ##msg);                              \
    }                                                                   \
  }

#define NOT_NULL(expr) \
  COMMON_NAMESPACE::NotNull(__FILE__, __LINE__, __func__, (expr), #expr)

namespace chloros {

namespace common {

enum class LogLevel { kInfo, kWarn, kDebug, kFatal };

void Log(LogLevel, char const*, int, char const*, char const*, ...)
    __attribute__((format(printf, 5, 6)));

void AssertFail(char const*, int, char const*, char const*, char const* = 0,
                ...) __attribute__((format(printf, 5, 6), noreturn));

template <typename T>
T* NotNull(char const* file, int line, char const* func, T* t,
           char const* expr) {
  if (UNLIKELY(t == nullptr)) {
    AssertFail(file, line, func, expr, "Must not be `nullptr`.");
  }
  return t;
}

std::string FormatString(char const*, ...)
    __attribute__((format(printf, 1, 2)));

std::string FormatStringVariadic(char const*, std::va_list);

class AssertionError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};  // class AssertionError

class FatalError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};  // class FatalError

void Ignore(...) __attribute__((unused));

inline void Ignore(...) {}

}  // namespace common

}  // namespace chloros

#endif  // CHLOROS_SRC_COMMON_H_
