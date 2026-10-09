#include "freewake/cli/App.hpp"

int main(int argc, char** argv) {
    FreeWake::CLIApp app(argc, argv);
    return app.run();
}
