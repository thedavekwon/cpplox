#include <string>
#include <format>
#include <memory>
#include <iostream>
#include <sstream>

#include <driver/driver.h>

#include <catch2/catch_test_macros.hpp>

using namespace cpplox;

TEST_CASE("Class") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/class.lox");
    REQUIRE(ss.str() == "<class MyClass>\n\"0\"\n\"1\"\n\"hello\"\n<instance of <class MyClass>>\n");
}

TEST_CASE("ComplexReturn") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/complex_return.lox");
    REQUIRE(ss.str() == "1\n2\n3\n");
}

TEST_CASE("Control") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/control.lox");
    REQUIRE(ss.str() == "0\n1\n1\n2\n3\n5\n8\n13\n21\n34\n55\n89\n144\n233\n377\n610\n987\n1597\n2584\n4181\n6765\n");
}

TEST_CASE("Fib") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/fib.lox");
    REQUIRE(ss.str() == "0\n1\n1\n2\n3\n5\n8\n13\n21\n34\n55\n89\n144\n233\n377\n610\n987\n1597\n2584\n4181\n");
}

TEST_CASE("Fn") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/fn.lox");
    REQUIRE(ss.str() == "<fn IDENTIFIER add 1>\n3\n<fn IDENTIFIER sayHi 8>\n\"Hi, Dear Reader!\"\n");
}

TEST_CASE("LocalFunction") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/local_function.lox");
    REQUIRE(ss.str() == "1\n2\n");
}

TEST_CASE("Scope") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/scope.lox");
    REQUIRE(ss.str() == "\"inner a\"\n\"outer b\"\n\"global c\"\n\"outer a\"\n\"outer b\"\n\"global c\"\n\"global a\"\n\"global b\"\n\"global c\"\n");
}

TEST_CASE("StaticScope") {
    std::stringstream ss;
    InterpreterDriver driver(ss);
    driver.runScript("/Users/dohyungkwon/Projects/cpplox/sample/static_scope.lox");
    REQUIRE(ss.str() == "\"global\"\n\"global\"\n");
}
