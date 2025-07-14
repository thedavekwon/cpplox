#include <print>

#include <driver/driver.h>

int main(int argc, char* argv[]) {
    cpplox::InterpreterDriver driver;

    if (argc > 2) {
        std::print("Usage: cpplox [script]\n");
    } else if (argc == 2) {
        std::print("Running {}\n", argv[1]);
        driver.runScript(argv[1]);
    } else {
        std::print("Running REPL mode.\n");
        driver.runPrompt();
    }
    return 0;
}