#pragma once

#include "../../output/output.h"
#include <string>
#include <vector>

class BuiltinCommand
{
  private:
    TokenParser m_parser;

    std::string m_name;
    std::string m_description;

    [[nodiscard]] virtual int Process(const std::vector<Token> &tokens) const;

  protected:
    Output *m_output;

  public:
    BuiltinCommand(std::string name, std::string description, Output *output);

    [[nodiscard]] virtual bool IsCommand(const std::string &command) const;
    [[nodiscard]] virtual bool NameStartWith(const std::string &partial) const;

    [[nodiscard]] const std::string &GetName() const;
    [[nodiscard]] const std::string &GetDescription() const;

    [[nodiscard]] virtual int Execute(const std::vector<Token> &tokens) const;
};
