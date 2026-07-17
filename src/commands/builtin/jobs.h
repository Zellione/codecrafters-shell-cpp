#pragma once

#include "../../registries/jobs.h"
#include "command.h"

class JobsCommand : public BuiltinCommand
{
  private:
    JobsRegistry &m_registry;

    [[nodiscard]] int Process(const Ast::Command &comm) const override;

  public:
    JobsCommand(JobsRegistry &registry);

    [[nodiscard]] static std::string
    GenerateProcessStatus(BackgroundJobStatus status);
};
