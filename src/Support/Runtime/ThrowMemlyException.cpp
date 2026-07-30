#include "Support/Runtime/ThrowMemlyException.hpp"

#include <initializer_list>
#include <source_location>
#include <string_view>

#include "Support/Runtime/MemlyException.hpp"

namespace Support::Runtime {
[[noreturn]] void ThrowMemlyException(const std::initializer_list<std::string_view> ErrorMessageInitializerList, const std::source_location& SourceLocation) {
    throw MemlyException{ ErrorMessageInitializerList, SourceLocation };
}
}
