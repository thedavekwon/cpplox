#pragma once

#include <type_traits>

namespace cpplox {

template <typename T, typename = void>
constexpr bool is_derefable_v = false;
template <typename T>
constexpr bool is_derefable_v<T, std::void_t<decltype(*std::declval<T>())>> = true;

template <typename T, typename Container>
struct is_contained_in;
template <typename T, template <typename...> typename Container, typename... Ts>
struct is_contained_in<T, Container<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};
template <typename T, typename Container>
constexpr bool is_contained_in_v = is_contained_in<T, Container>::value;

} // cpplox