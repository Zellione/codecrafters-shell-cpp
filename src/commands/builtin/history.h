#pragma once

#include "../../registries/history.h"
#include "command.h"

class HistoryCommand : public BuiltinCommand
{
  private:
    HistoryRegistry *m_registry;

    std::string m_histfile;

    [[nodiscard]] int Process(const Ast::Command &comm) const override;

    void AppendFileToHistory(const std::string &filename) const;
    void AppendHistoryToFile(const std::string &filename, bool append) const;

  public:
    HistoryCommand(HistoryRegistry *registry);

    void SetHistoryFile(const std::string &histfile);
};
