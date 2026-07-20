#include "commands/builtin/registry.h"
#include "commands/executor.h"
#include "commands/external/command.h"
#include "registries/complete.h"
#include "registries/history.h"

class Shell
{
  private:
    bool m_exit_shell;
    bool m_line_ready;
    bool m_main_process;

    std::string m_current_input;
    std::string m_last_prompt;

    HistoryRegistry *m_history_registry;
    BuiltinRegistry m_registry;
    Output m_output;

    std::vector<std::string> m_autocomplete;

    ExternalCommand m_external_comm;

    Executor m_executor;
    CompleteRegistry *m_complete_registry;

    Shell();

    // BEGIN Autocomplete
    [[nodiscard]] std::vector<std::string>
    CollectAutocompletes(const std::string &partial, Ast::Node *nodes);

    static std::vector<std::string>
    CollectAutocompleteInPath(const std::string &partial);

    [[nodiscard]] std::vector<std::string>
    CollectAutocompleteBuiltin(const std::string &partial) const;

    static std::vector<std::string>
    CollectAutocompleteInDir(const std::string &partial,
                             const Ast::Command &comm);

    [[nodiscard]] std::string
    LongestCommonPrefix(const std::string &partial) const;
    // END Autocomplete

  public:
    Shell(Shell &) = delete;
    void operator=(const Shell &) = delete;
    ~Shell();

    static Shell &Instance()
    {
        static Shell s;
        return s;
    }

    void run();

    static int ReapBackgroundJobs();

    static int TabAutoComplete(int count, int key);
    static int TabAutoCompleteMulti(int count, int key);

    static void LineHandler(char *line);

    void ExitShell(bool exit);
};
