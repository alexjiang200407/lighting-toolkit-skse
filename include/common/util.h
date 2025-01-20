#include <type_traits>

#define ASSERT_BASE(super, sub) static_assert(std::is_base_of<sub, super>::value, "Class " #super " must be a superclass of " #sub)
