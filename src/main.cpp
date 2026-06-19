#include <iostream>
#include <string>

bool builtin(std::string& user_input) {
    if (user_input == "exit")
      exit(0);

    if (user_input.find("echo") == 0) {
        std::cout << user_input.substr(5) << std::endl;
        return true;
    }

    return false;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";

    std::string user_input;
    std::getline(std::cin, user_input);


    if(!builtin(user_input))
        std::cout << user_input << ": command not found" << std::endl;
  }
}
