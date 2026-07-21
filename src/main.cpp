#include <iostream>

#include "readline/readline.h"
#include "shell.h"

int main()
{
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Register readline keys
    rl_bind_key('\t', Shell::TabAutoComplete);
    rl_bind_keyseq("\e[A", Shell::UpArrow);
    rl_bind_keyseq("\e[B", Shell::DownArrow);

    Shell &shell = Shell::Instance();

    shell.run();

    return 0;
}
