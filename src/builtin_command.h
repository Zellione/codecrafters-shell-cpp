#pragma once

#include "output/output.h"
#include "parser/token_parser.h"
#include <string>
#include <vector>

class BuiltinCommand {
  private:
    TokenParser m_parser;

    std::string m_name;
    std::string m_description;

    virtual void Process(std::vector<Token> tokens) const {};

  protected:
    Output *m_output;

  public:
    BuiltinCommand(std::string name, std::string description, Output *output);

    virtual bool IsCommand(std::string command) const;

    std::string GetDescription() const;

    virtual void Execute(std::string commandline) const;
};
