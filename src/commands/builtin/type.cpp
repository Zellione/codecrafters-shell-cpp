#include "type.h"
#include "../../helper.h"
#include "command.h"

TypeCommand::TypeCommand(const BuiltinRegistry *registry, Output *output)
    : BuiltinCommand("type", "type is a shell builtin", output),
      m_registry(registry)
{
}

int TypeCommand::Process(const std::vector<Token> &tokens) const
{
    const BuiltinCommand *command =
        m_registry->FindCommandInArguments(tokens, 1);

    if (command != nullptr)
    {
        m_output->Put(tokens, std::format("{}\n", command->GetDescription()),
                      OutputTarget::STDOUT);
        return 0;
    }

    std::string type_check = !tokens.empty() ? tokens[1].token : "";
    std::string executable = find_executable(type_check);
    if (!executable.empty())
    {
        m_output->Put(tokens, std::format("{} is {}\n", type_check, executable),
                      OutputTarget::STDOUT);
        return 0;
    }

    m_output->Put(tokens, std::format("{}: not found\n", type_check),
                  OutputTarget::STDOUT);

    return 0;
}
