#include <algorithm>
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// ANSI color codes
namespace Color {
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string DIM = "\033[2m";
const std::string ITALIC = "\033[3m";
const std::string UNDERLINE = "\033[4m";

const std::string BLACK = "\033[30m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

const std::string BG_BLACK = "\033[40m";
const std::string BG_RED = "\033[41m";
const std::string BG_GREEN = "\033[42m";
const std::string BG_YELLOW = "\033[43m";
const std::string BG_BLUE = "\033[44m";
const std::string BG_MAGENTA = "\033[45m";
const std::string BG_CYAN = "\033[46m";
const std::string BG_WHITE = "\033[47m";

// Bright variants
const std::string BRIGHT_BLACK = "\033[90m";
const std::string BRIGHT_RED = "\033[91m";
const std::string BRIGHT_GREEN = "\033[92m";
const std::string BRIGHT_YELLOW = "\033[93m";
const std::string BRIGHT_BLUE = "\033[94m";
const std::string BRIGHT_MAGENTA = "\033[95m";
const std::string BRIGHT_CYAN = "\033[96m";
const std::string BRIGHT_WHITE = "\033[97m";
} // namespace Color

// Nerd font icons (assuming terminal has Nerd Fonts installed)
namespace Icon {
const std::string UBUNTU = "\uef72";  // Ubuntu logo
const std::string FEDORA = "\ue7d9";  // Fedora logo
const std::string ALPINE = "\uf300";  // Alpine logo
const std::string WSL    = "\ue62a";  // Windows (WSL) logo
const std::string ARCH = "\ue732";    // Arch logo
const std::string SUSE = "\uef6d";    // SUSE logo
const std::string GENTOO = "\ue7e6";  // Gentoo logo
const std::string DEBIAN = "\ue77d";  // Debian logo
const std::string CENTOS = "\ue78a";  // CentOS logo
const std::string COMMAND = "\uf4b5"; // Command icon
const std::string PACKAGE = "\ueb29"; // Package icon
const std::string INFO = "\uea74";    // Info icon
const std::string SEARCH = "\uea6d";  // Magnifying glass
const std::string CHECK = "\ueab2";   // Check mark
const std::string WARN = "\uf071";    // Warning
const std::string ERROR = "\uea87";   // Error
const std::string ARROW = "\uea9c";   // Right arrow
const std::string SHELL = "\ue691";   // Shell
} // namespace Icon

// Get icon for a specific distro
std::string getDistroIcon(const std::string &distro) {
  if (distro == "ubuntu" || distro == "debian")
    return Icon::UBUNTU;
  if (distro == "fedora" || distro == "centos" || distro == "rhel")
    return Icon::FEDORA;
  if (distro == "alpine")
    return Icon::ALPINE;
  if (distro == "arch")
    return Icon::ARCH;
  if (distro == "suse" || distro == "opensuse")
    return Icon::SUSE;
  if (distro == "gentoo")
    return Icon::GENTOO;

  if (distro == "kali")
    return Icon::DEBIAN;
  if (distro == "windows")
    return Icon::WSL;

  return Icon::PACKAGE;
}

// Get color for a specific distro
std::string getDistroColor(const std::string &distro) {
  if (distro == "ubuntu" || distro == "debian")
    return Color::BRIGHT_MAGENTA;
  if (distro == "fedora" || distro == "centos" || distro == "rhel" ||
      distro == "kali")
    return Color::BRIGHT_BLUE;
  if (distro == "alpine")
    return Color::BRIGHT_CYAN;
  if (distro == "arch")
    return Color::BRIGHT_CYAN;
  if (distro == "suse" || distro == "opensuse")
    return Color::BRIGHT_GREEN;
  if (distro == "gentoo")
    return Color::BRIGHT_MAGENTA;
  return Color::BRIGHT_WHITE;
}

// Callback function for cURL to write response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            std::string *userp) {
  userp->append((char *)contents, size * nmemb);
  return size * nmemb;
}

class CommandInfo {
private:
  std::string name;
  std::map<std::string, std::string> installCommands;

public:
  CommandInfo(const std::string &cmdName) : name(cmdName) {}

  void addDistroCommand(const std::string &distro, const std::string &command) {
    installCommands[distro] = command;
  }

  std::string getName() const { return name; }

  std::string getCommand(const std::string &distro) const {
    auto it = installCommands.find(distro);
    if (it != installCommands.end()) {
      return it->second;
    }
    return "No installation command found for this distribution";
  }

  bool hasDistro(const std::string &distro) const {
    return installCommands.find(distro) != installCommands.end();
  }

  // Allow accessing distro commands using arrow operator syntax
  std::string operator->*(const char *distro) const {
    if (std::string(distro) == "all") {
      return getAllCommands();
    }
    return getCommand(distro);
  }

  // Get all commands as a single string
  std::string getAllCommands() const {
    std::stringstream ss;
    for (const auto &[distro, cmd] : installCommands) {
      ss << distro << ": " << cmd << "\n";
    }
    return ss.str();
  }

  // List all supported distros
  std::vector<std::string> getSupportedDistros() const {
    std::vector<std::string> distros;
    for (const auto &entry : installCommands) {
      distros.push_back(entry.first);
    }
    return distros;
  }

  // Map of all commands
  const std::map<std::string, std::string> &getAllCommandsMap() const {
    return installCommands;
  }

  // Pretty format a command
  std::string formatCommand(const std::string &command) const {
    std::string result = command;

    // Highlight sudo in red
    std::regex sudoRegex("(sudo)");
    result =
        std::regex_replace(result, sudoRegex, Color::RED + "$1" + Color::RESET);

    // Highlight apt/dnf/pacman commands in blue
    std::regex packageManagerRegex(
        "(apt|apt-get|dnf|yum|pacman|apk|zypper|emerge)");
    result = std::regex_replace(result, packageManagerRegex,
                                Color::BLUE + "$1" + Color::RESET);

    // Highlight install commands in green
    std::regex installRegex("(install|add|get)");
    result = std::regex_replace(result, installRegex,
                                Color::GREEN + "$1" + Color::RESET);

    // Highlight package name in cyan
    std::regex packageRegex("(" + name + ")");
    result = std::regex_replace(
        result, packageRegex, Color::CYAN + Color::BOLD + "$1" + Color::RESET);

    return result;
  }

  // Custom output stream operator with pretty formatting
  friend std::ostream &operator<<(std::ostream &os, const CommandInfo &cmdInfo);
};

std::ostream &operator<<(std::ostream &os, const CommandInfo &cmdInfo) {
  // Create a box around the output
  int width = 80;

  os << '\n';

  // Command header
  os << Color::BOLD << Color::BRIGHT_WHITE << "Command: " << Color::RESET
     << Color::BRIGHT_YELLOW << cmdInfo.name << Color::RESET << '\n';

  // Installation commands section
  os << Color::BOLD << Color::BRIGHT_WHITE
     << "Installation Commands:" << Color::RESET << '\n';

  if (cmdInfo.installCommands.empty()) {
    os << Icon::WARN << " "
       << Color::YELLOW << "No installation commands found" << Color::RESET
       << '\n';
  } else {
    for (const auto &[distro, cmd] : cmdInfo.installCommands) {
      std::string distroIcon = getDistroIcon(distro);
      std::string distroColor = getDistroColor(distro);

      // Format distro with icon and color
      os << distroIcon << " " << distroColor << std::left << std::setw(10)
         << distro << Color::RESET;

      // Format the command
      std::string formattedCmd = cmdInfo.formatCommand(cmd);
      int maxCmdLength = width - 12; // Account for left padding and distro name

      if (cmd.length() <= maxCmdLength) {
        os << Icon::ARROW << " " << formattedCmd << '\n';
      } else {
        // Handle long commands by wrapping them
        os << Icon::ARROW << " " << formattedCmd.substr(0, maxCmdLength - 3)
           << "..." << '\n';

        // Output the rest on next lines with proper indentation
        size_t pos = maxCmdLength - 3;
        while (pos < formattedCmd.length()) {
          size_t chunk = std::min(static_cast<size_t>(maxCmdLength - 12),
                                  formattedCmd.length() - pos);
          os << std::string(12, ' ') << formattedCmd.substr(pos, chunk) << '\n';
          pos += chunk;
        }
      }
    }
  }

  os << '\n';
  return os;
}

class Commandy {
private:
  CURL *curl;
  std::string baseUrl;

  // Parse HTML response based on the actual structure
  CommandInfo parseResponse(const std::string &response,
                            const std::string &cmdName) {
    CommandInfo cmdInfo(cmdName);

    // Based on the HTML you shared, we need to look for divs with class
    // "command-install install-DISTRO"
    std::regex installDivRegex(
        "<div class=\"command-install install-([^\"]+)\" "
        "data-os=\"([^\"]+)\">[\\s\\S]*?<code>([^<]+)</code>[\\s\\S]*?</div>");

    std::string::const_iterator searchStart(response.cbegin());
    std::string::const_iterator searchEnd(response.cend());
    std::smatch divMatch;

    while (
        std::regex_search(searchStart, searchEnd, divMatch, installDivRegex)) {
      std::string distroClass = divMatch[1].str();
      std::string distroName = divMatch[2].str();
      std::string command = divMatch[3].str();

      // Map distro classes to our standard names
      std::string distroKey;
      if (distroClass == "ubuntu" || distroClass == "debian") {
        distroKey = "ubuntu";
      } else if (distroClass == "fedora" || distroClass == "centos" ||
                 distroClass == "rhel") {
        distroKey = "fedora";
      } else if (distroClass == "alpine") {
        distroKey = "alpine";
      } else if (distroClass == "arch") {
        distroKey = "arch";
      } else if (distroClass == "suse" || distroClass == "opensuse") {
        distroKey = "suse";
      } else if (distroClass == "gentoo") {
        distroKey = "gentoo";
      } else {
        // Use the original class name for others
        distroKey = distroClass;
      }

      cmdInfo.addDistroCommand(distroKey, command);
      searchStart = divMatch.suffix().first;
    }

    // If no commands found, try a fallback method
    if (cmdInfo.getSupportedDistros().empty()) {
      std::regex codeBlockRegex("<dt[^>]*>[^<]*([A-Za-z]+)[^<]*</"
                                "dt>\\s*<dd[^>]*>\\s*<code>([^<]+)</code>");

      searchStart = response.cbegin();
      std::smatch codeMatch;

      while (std::regex_search(searchStart, searchEnd, codeMatch,
                               codeBlockRegex)) {
        std::string distroName = codeMatch[1].str();
        std::string command = codeMatch[2].str();

        // Normalize distro name
        std::string distroKey = distroName;
        std::transform(distroKey.begin(), distroKey.end(), distroKey.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (distroKey.find("ubuntu") != std::string::npos ||
            distroKey.find("debian") != std::string::npos) {
          distroKey = "ubuntu";
        } else if (distroKey.find("fedora") != std::string::npos ||
                   distroKey.find("centos") != std::string::npos) {
          distroKey = "fedora";
        } else if (distroKey.find("alpine") != std::string::npos) {
          distroKey = "alpine";
        } else if (distroKey.find("arch") != std::string::npos) {
          distroKey = "arch";
        } else if (distroKey.find("suse") != std::string::npos) {
          distroKey = "suse";
        } else if (distroKey.find("gentoo") != std::string::npos) {
          distroKey = "gentoo";
        }

        cmdInfo.addDistroCommand(distroKey, command);
        searchStart = codeMatch.suffix().first;
      }
    }

    return cmdInfo;
  }

public:
  Commandy() : baseUrl("https://command-not-found.com/") {
    curl = curl_easy_init();
    if (!curl) {
      throw std::runtime_error("Failed to initialize cURL");
    }
  }

  ~Commandy() {
    if (curl) {
      curl_easy_cleanup(curl);
    }
  }

  CommandInfo search_for(const std::string &command) {
    if (!curl) {
      throw std::runtime_error("cURL not initialized");
    }

    std::string url = baseUrl + command;
    std::string readBuffer;

    // Display search indicator
    std::cout << Icon::SEARCH << " " << Color::BRIGHT_WHITE << "Searching for "
              << Color::BRIGHT_YELLOW << command << Color::RESET << "..."
              << std::endl;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
                     "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                     "(KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << Icon::ERROR << " " << Color::BRIGHT_RED
                << "Error: " << curl_easy_strerror(res) << Color::RESET
                << std::endl;
      throw std::runtime_error("cURL request failed: " +
                               std::string(curl_easy_strerror(res)));
    }

    // Success indicator
    std::cout << Icon::CHECK << " " << Color::BRIGHT_GREEN
              << "Found information for " << Color::BRIGHT_YELLOW << command
              << Color::RESET << std::endl;

    return parseResponse(readBuffer, command);
  }
};
