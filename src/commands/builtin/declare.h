#include "command.h"

class DeclareCommand : public BuiltinCommand 
{
  private:
    [[nodiscard]] int Process(const Ast::Command &comm) const override;

  public:
    DeclareCommand();
};
