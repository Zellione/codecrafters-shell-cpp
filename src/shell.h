#include "commands/builtin/registry.h"
#include "commands/external/command.h"
#include "registries/complete.h"
#include "registries/jobs.h"

class Shell
{
  private:
    bool m_exit_shell;
    bool m_line_ready;
    bool m_main_process;

    std::string m_current_input;
    std::string m_last_prompt;

    BuiltinRegistry m_registry;
    Output m_output;

    std::vector<std::string> m_autocomplete;

    JobsRegistry *m_jobs_registry;

    ExternalCommand m_external_comm;
    CompleteRegistry *m_complete_registry;

    Shell();

    // BEGIN Autocomplete
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
    // END Autocomplete

    // BEGIN Task Execution
    [[nodiscard]] int ExecuteCommand(const std::vector<Token> &command) const;
    [[nodiscard]] int
    ExecuteCommandChain(const std::vector<std::vector<Token>> &commands) const;
    [[nodiscard]] int ExecuteBackgroundCommandChain(
        const std::vector<std::vector<Token>> &commands);
    // END Task Execution

    // BEGIN Coomand Chain Parsing
    [[nodiscard]] static bool
    HasBackgroundFlag(const std::vector<Token> &tokens);
    [[nodiscard]] static std::vector<std::vector<Token>>
    SplitCommandChain(const std::vector<Token> &tokens);
    [[nodiscard]] static std::string
    GetCommandline(const std::vector<std::vector<Token>> &commands);
    // END Command Chain Parsing

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
