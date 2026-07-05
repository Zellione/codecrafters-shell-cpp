#pragma once

#include "../../registries/jobs.h"
#include "command.h"

class JobsCommand : public BuiltinCommand
{
  private:
    JobsRegistry &m_registry;

    [[nodiscard]] int Process(const std::vector<Token> &tokens) const override;
    [[nodiscard]] static std::string
    GenerateProcessStatus(BackgroundJobStatus status);

  public:
    JobsCommand(Output *output, JobsRegistry &registry);
};
