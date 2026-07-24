#include "declare.h"

using Ast::Command;

DeclareCommand::DeclareCommand() : BuiltinCommand("declare") {}

int DeclareCommand::Process(const Command &comm) const { return 0; }
