#pragma once

#include "../../registries/complete.h"
#include "command.h"

class CompleteCommand : public BuiltinCommand
{
  private:
    CompleteRegistry *m_completeRegistry;

    [[nodiscard]] int Process(const Ast::Command &comm) const override;

    void Print(const Ast::Command &comm, const std::string &name) const;
    void Create(const std::string &name, const std::string &completion) const;

  public:
    CompleteCommand(CompleteRegistry *completeRegistry);
};
