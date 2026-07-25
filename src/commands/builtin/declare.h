#include "command.h"

#include <map>

class DeclareCommand : public BuiltinCommand
{
  private:
    std::map<std::string, std::string> m_variables;

    [[nodiscard]] int Process(const Ast::Command &comm) override;

    static bool IsValidVariableName(const std::string &name);

  public:
    DeclareCommand();
};
