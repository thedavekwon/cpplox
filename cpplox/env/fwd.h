#pragma once

#include <memory>
#include <variant>

namespace cpplox {

class Interpreter;
class Environment;

using EnvironmentPtr = std::shared_ptr<class Environment>;
using FunctionPtr = std::shared_ptr<class Function>;
using NativeFunctionPtr = std::shared_ptr<class NativeFunction>;
using ClassPtr = std::shared_ptr<class Class>;
using InstancePtr = std::shared_ptr<class Instance>;

using Object = std::variant<std::nullptr_t, bool, double, std::string, FunctionPtr, NativeFunctionPtr, ClassPtr, InstancePtr>;

} // cpplox