#pragma once

namespace FreeWake {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual int execute() = 0;
};

} // namespace FreeWake
