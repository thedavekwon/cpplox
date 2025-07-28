#include <env/object.h>

namespace cpplox {

Object NativeFunction::call(Interpreter* i, std::vector<Object> arguments) {
    return call_(i, std::move(arguments));
}

} // cpplox