#include "commands/builtin/registry.h"
#include "commands/external/command.h"

class Shell
{
  private:
    BuiltinRegistry m_registry;
    Output m_output;
    ExternalCommand m_external_comm;
    std::string m_current_input;

    Shell();

  public:
    static Shell &Instance()
    {
        static Shell s;
        return s;
    }

    void run();

    static int TabAutoComplete(int count, int key);
};
