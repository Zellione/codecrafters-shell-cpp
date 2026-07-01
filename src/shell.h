#include "commands/builtin/registry.h"
#include "commands/external/command.h"
#include "registries/complete.h"
#include "registries/jobs.h"

class Shell
{
  private:
    BuiltinRegistry m_registry;
    Output m_output;
    std::string m_currentInput;

    std::vector<std::string> m_autocomplete;
    std::string m_lastprompt;

    JobsRegistry *m_jobsRegistry;

    ExternalCommand m_externalComm;
    CompleteRegistry *m_completeRegistry;

    Shell();

    [[nodiscard]] std::vector<std::string>
    CollectAutocompletes(const std::string &partial,
                         const std::vector<Token> &tokens);

    static std::vector<std::string>
    CollectAutocompleteInPath(const std::string &partial);

    [[nodiscard]] std::vector<std::string>
    CollectAutocompleteBuiltin(const std::string &partial) const;

    static std::vector<std::string>
    CollectAutocompleteInDir(const std::string &partial,
                             const std::vector<Token> &tokens);

    [[nodiscard]] std::string
    LongestCommonPrefix(const std::string &partial) const;

  public:
    ~Shell();

    static Shell &Instance()
    {
        static Shell s;
        return s;
    }

    void run();

    static int TabAutoComplete(int count, int key);
    static int TabAutoCompleteMulti(int count, int key);
};
