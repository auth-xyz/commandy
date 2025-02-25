#include "../libs/cmdy.hpp"
#include <clicky/clicky.hpp>

#include <iostream>

int main(int argc, char *argv[]) {
  clicky cli("{program} : [tool]");
  Commandy api;

  cli.bulk_add_arguments(
      {{"command", "c", "command to look for", true},
       {"distro", "d",
        "show only a specific distro (required if you don't specify --list)",
        false}});
  cli.add_option("list", "la", "Lists all possible distros", false);
  cli.parse(argc, argv);

  try {
    CommandInfo command = api.search_for(cli.get_argument("command"));

    if (!cli.has_option("list")) {
      std::string distro = cli.get_argument("distro");
      std::cout << "\n" << getDistroIcon(distro) << distro << ": " << command.getCommand(distro) << "\n";
    } else {
      auto distros = command.getSupportedDistros();
      if (distros.empty()) {
        std::cout << "This tool was not found";
        return 1;
      }
      std::cout << "\n";
      for (const auto &d : distros) {
        std::cout << "  "
                  << Icon::ARROW
                  << " "
                  << getDistroColor(d)
                  << getDistroIcon(d)
                  << " "
                  << d 
                  << ": " 
                  << Color::RESET
                  << Color::DIM
                  << command.getCommand(d) 
                  << Color::RESET
                  << "\n";

      }
    }

  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
  }
}
