#include "commands/builtin/registry.h"
#include "commands/external/command.h"

class Shell
{
  private:
    BuiltinRegistry m_registry;
    Output m_output;
    ExternalCommand m_external_comm;
    std::string m_current_input;

    std::vector<std::string> m_autocomplete;
    std::string m_lastprompt;

    Shell();

    [[nodiscard]] std::vector<std::string>
    CollectAutocompletes(const std::string &partial) const;

    [[nodiscard]] std::string
    LongestCommonPrefix(const std::string &partial) const;

  public:
    static Shell &Instance()
    {
        static Shell s;
        return s;
    }

    void run();

    static int TabAutoComplete(int count, int key);
    static int TabAutoCompleteMulti(int count, int key);
};
