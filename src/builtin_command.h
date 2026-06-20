#pragma once

#include <string>
#include <vector>

class BuiltinCommand {
  private:
    std::string m_name;
    std::string m_description;

    virtual void Process(std::vector<std::string> arguments) const {};

  public:
    BuiltinCommand(std::string name, std::string description);

    virtual bool IsCommand(std::string command) const;

    std::string GetDescription() const;

    virtual void Execute(std::string commandline) const;
};
