#include <env/object.h>

namespace cpplox {

Object NativeFunction::call(Interpreter* i, std::vector<Object> arguments) {
    return call_(i, std::move(arguments));
}

FunctionPtr Function::bind(InstancePtr instance) {
    EnvironmentPtr env = std::make_shared<Environment>(closure_);
    env->define("this", instance);
    return std::make_shared<Function>(env, declaration_, isInit_);
}

size_t Class::arity() const {
    if (auto it = methods_.find("init"); it != methods_.end()) {
        return it->second->arity();
    }
    return 0;
}

} // cpplox