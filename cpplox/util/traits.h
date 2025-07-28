#pragma once

#include <type_traits>

namespace cpplox {

template <typename T, typename = void>
constexpr bool is_derefable_v = false;
template <typename T>
constexpr bool is_derefable_v<T, std::void_t<decltype(*std::declval<T>())>> = true;

} // cpplox