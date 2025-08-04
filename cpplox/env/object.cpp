#include <env/object.h>

namespace cpplox {

Object NativeFunction::call(Interpreter* i, std::vector<Object> arguments) {
    return call_(i, std::move(arguments));
}

Object Class::call(Interpreter* i, std::vector<Object> arguments) {
    return std::make_shared<Instance>(shared_from_this());
}

size_t Class::arity() const {
    if (auto it = methods_.find("init"); it != methods_.end()) {
        return it->second->arity();
    }
    return 0;
}

} // cpplox