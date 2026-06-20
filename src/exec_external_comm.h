#pragma once

#include <string>

class ExecExternalCommand {
  public:
    bool Exec(std::string commandline) const;
};
