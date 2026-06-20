#pragma once

#include "builtin_command.h"
#include "builtin_registry.h"

class TypeCommand : public BuiltinCommand {
private:
  virtual void Process(std::vector<std::string> arguments) const override;

  std::string FindExecutable(std::string command) const;
  std::vector<std::string> GetValidPathFolders() const;
  bool IsExecutableFile(const std::string &filepath) const;

  const BuiltinRegistry *m_registry;

public:
  TypeCommand(const BuiltinRegistry *registry);
};
