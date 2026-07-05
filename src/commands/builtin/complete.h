#pragma once

#include "../../registries/complete.h"
#include "command.h"

class CompleteCommand : public BuiltinCommand
{
  private:
    CompleteRegistry *m_completeRegistry;

    [[nodiscard]] int Process(const std::vector<Token> &tokens) const override;

    void Print(const std::vector<Token> &tokens, const std::string &name) const;
    void Create(const std::string &name, const std::string &completion) const;

  public:
    CompleteCommand(Output *output, CompleteRegistry *completeRegistry);
};
