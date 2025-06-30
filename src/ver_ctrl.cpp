#include "ver_ctrl.hpp"
#include "utils.hpp"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <set>

std::vector<IgnoreRule> ViciIgnore::parse(const std::filesystem::path& repoPath) {
    std::vector<IgnoreRule> rules;
    std::ifstream ignoreFile(repoPath / ".viciignore");
    if (!ignoreFile) return rules;
    std::string line;
    while (std::getline(ignoreFile, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty()) continue;
        if (line.size() > 2 && line.substr(line.size() - 2) == "/*") {
            rules.push_back({line.substr(0, line.size() - 2), IgnoreRule::Type::FolderFiles});
        } else if (!line.empty() && line.back() == '/') {
            rules.push_back({line.substr(0, line.size() - 1), IgnoreRule::Type::Folder});
        } else {
            rules.push_back({line, IgnoreRule::Type::File});
        }
    }
    return rules;
}

bool ViciIgnore::isIgnored(const std::vector<IgnoreRule>& rules, const std::filesystem::path& relPath, bool isDir) {
    std::string relStr = relPath.generic_string();
    for (const auto& rule : rules) {
        if (rule.type == IgnoreRule::Type::File) {
            if (relStr == rule.pattern || relPath.filename() == rule.pattern) return true;
        } else if (rule.type == IgnoreRule::Type::Folder) {
            if (relStr == rule.pattern || relStr.rfind(rule.pattern + "/", 0) == 0) return true;
        } else if (rule.type == IgnoreRule::Type::FolderFiles) {
            if (!isDir && relPath.parent_path().generic_string() == rule.pattern) return true;
        }
    }
    return false;
}

std::filesystem::path VersionControl::getRepoPath(const std::string& repoName) {
    return getBaseDir() / "user_repos" / (repoName + ".curr");
}

std::filesystem::path VersionControl::getVersionsPath(const std::string& repoName) {
    return getBaseDir() / "user_repos" / repoName;
}

bool VersionControl::repoExists(const std::string& repoName) {
    auto repoPath = getRepoPath(repoName);
    return std::filesystem::exists(repoPath) && std::filesystem::is_directory(repoPath);
}

bool VersionControl::versionsFolderExists(const std::string& repoName) {
    auto versionsPath = getVersionsPath(repoName);
    return std::filesystem::exists(versionsPath) && std::filesystem::is_directory(versionsPath);
}

bool VersionControl::checkRepoSelected(std::string& currentRepo) {
    if (currentRepo.empty()) {
        std::ifstream in(getBaseDir() / "user_repos" / ".vicicurr");
        if (in) std::getline(in, currentRepo);
    }
    if (currentRepo.empty()) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        std::cout << "Error: No repository selected.\n";
        return false;
    }
    return true;
}

bool VersionControl::checkArgsSize(const std::vector<std::string>& args, size_t minSize, const std::string& usage) {
    if (args.size() < minSize) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        std::cout << "Error: Usage: " << usage << "\n";
        return false;
    }
    return true;
}

bool VersionControl::selectRepo(const std::vector<std::string>& args, std::string& currentRepo) {
    if (!checkArgsSize(args, 2, "cr <repo_name>")) return false;
    std::string repoName = args[1];
    if (repoExists(repoName)) {
        currentRepo = repoName;
        std::cout << "Switched to repository: " << repoName << "\n";
        std::ofstream(getBaseDir() / "user_repos" / ".vicicurr") << repoName;
        return true;
    }
    #ifdef _WIN32
        std::cout << '\a';
    #endif
    std::cout << "Error: Repository '" << repoName << "' does not exist.\n";
    currentRepo.clear();
    return false;
}

bool VersionControl::createRepo(const std::vector<std::string>& args) {
    if (!checkArgsSize(args, 2, "new <repo_name>")) return false;
    std::string repoName = args[1];
    auto repoPath = getRepoPath(repoName);
    auto versionsPath = getVersionsPath(repoName);
    if (repoExists(repoName)) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        std::cout << "Error: Repository '" << repoName << "' already exists.\n";
        return false;
    }
    try {
        std::filesystem::create_directories(repoPath);
        std::filesystem::create_directories(versionsPath);
        std::ofstream initFile(repoPath / "README.txt");
        initFile << "Repository: " << repoName << "\nCreated with Vici.\n";
        initFile.close();
        std::cout << "Repository '" << repoName << "' created successfully.\n";
        std::cout << "Versions folder '" << versionsPath.string() << "' created successfully.\n";
        return true;
    } catch (const std::exception& e) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        std::cout << "Error: Failed to create repository: " << e.what() << "\n";
        return false;
    }
}

bool VersionControl::commitRepo(const std::vector<std::string>& args, std::string& currentRepo, const std::string& in) {
    if (!checkRepoSelected(currentRepo)) return false;
    std::string message;
    if (args.size() > 1) {
        message = in.substr(in.find(args[1]));
    }
    if (commit(currentRepo, message)) {
        std::cout << "Committed current state of '" << currentRepo << "'.\n";
        return true;
    }
    #ifdef _WIN32
        std::cout << '\a';
    #endif
    std::cout << "Error: Commit failed.\n";
    return false;
}

bool VersionControl::logRepo(std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    auto versions = listVersions(currentRepo);
    if (versions.empty()) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        std::cout << "No commits found.\n";
        return false;
    }
    std::cout << "Commits for '" << currentRepo << "':\n";
    for (const auto& v : versions) {
        std::cout << v;
        std::string msg = getCommitMessage(currentRepo, v);
        if (!msg.empty()) std::cout << " - " << msg;
        std::cout << "\n";
    }
    return true;
}

bool VersionControl::checkoutRepo(const std::vector<std::string>& args, std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    if (!checkArgsSize(args, 2, "checkout <version>")) return false;
    if (checkout(currentRepo, args[1])) {
        std::cout << "Checked out version " << args[1] << " to '" << currentRepo << ".curr'.\n";
        return true;
    }
    #ifdef _WIN32
        std::cout << '\a';
    #endif
    std::cout << "Error: Checkout failed.\n";
    return false;
}

bool VersionControl::deleteRepoCmd(const std::vector<std::string>& args, std::string& currentRepo) {
    if (!checkArgsSize(args, 2, "delrepo <repo_name>")) return false;
    if (deleteRepo(args[1])) {
        std::cout << "Repository '" << args[1] << "' deleted.\n";
        if (currentRepo == args[1]) currentRepo.clear();
        return true;
    }
    #ifdef _WIN32
        std::cout << '\a';
    #endif
    std::cout << "Error: Failed to delete repository.\n";
    return false;
}

bool VersionControl::deleteVersionCmd(const std::vector<std::string>& args, std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    if (!checkArgsSize(args, 2, "delver <version>")) return false;
    if (deleteVersion(currentRepo, args[1])) {
        std::cout << "Version '" << args[1] << "' deleted.\n";
        return true;
    }
    #ifdef _WIN32
        std::cout << '\a';
    #endif
    std::cout << "Error: Failed to delete version.\n";
    return false;
}

bool VersionControl::statusRepo(std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    std::cout << "Current repo: " << currentRepo << "\n";
    std::string latest = getLatestVersion(currentRepo);
    if (!latest.empty()) {
        std::cout << "Latest commit: " << latest << "\n";
    } else {
    #ifdef _WIN32
        std::cout << '\a';
    #endif
        std::cout << "No commits yet.\n";
    }
    return true;
}

void VersionControl::help() {
    std::cout <<
        "Commands:\n"
        "  new <repo_name>         Create a new repository\n"
        "  cr <repo_name>          Change/select repository\n"
        "  commit [msg]            Commit current state with optional message\n"
        "  log                     List all commits for current repo\n"
        "  checkout <version>      Restore repo to a previous version\n"
        "  delrepo <repo_name>     Delete a repository and its versions\n"
        "  delver <version>        Delete a specific version from current repo\n"
        "  status                  Show current repo and latest commit\n"
        "  ls                      List files in the current repo's .curr folder\n"
        "  !!                      Execute the last command again\n"
        "  cls                     Clear the screen"
        "  help                    Show this help message\n"
        "  exit                    Exit the program\n";
}

bool VersionControl::listFiles(std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    auto repoPath = getRepoPath(currentRepo);
    if (!repoExists(currentRepo)) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        std::cout << "Error: Current repo folder does not exist.\n";
        return false;
    }
    std::cout << "Files in " << repoPath.string() << ":\n";
    InitUtils::listDirectoryContents(repoPath);
    return true;
}

void VersionControl::copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination, const std::vector<IgnoreRule>& ignoreRules) {
    std::filesystem::create_directories(destination);
    for (auto it = std::filesystem::recursive_directory_iterator(source); it != std::filesystem::recursive_directory_iterator(); ++it) {
        auto relPath = std::filesystem::relative(it->path(), source);
        bool isDir = it->is_directory();
        if (ViciIgnore::isIgnored(ignoreRules, relPath, isDir) || relPath.filename() == ".viciignore") {
            if (isDir) it.disable_recursion_pending();
            continue;
        }
        auto destPath = destination / relPath;
        if (isDir) {
            std::filesystem::create_directories(destPath);
        } else if (it->is_regular_file()) {
            std::filesystem::copy_file(it->path(), destPath, std::filesystem::copy_options::overwrite_existing);
        }
    }
}

bool VersionControl::commit(const std::string& repoName, const std::string& message) {
    try {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        #ifdef _WIN32
            localtime_s(&tm, &now_c);
        #else
            localtime_r(&now_c, &tm);
        #endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "v_%Y%m%d_%H%M%S");
        std::string versionFolder = oss.str();

        std::filesystem::path repoPath = getRepoPath(repoName);
        std::filesystem::path versionsPath = getVersionsPath(repoName);
        std::filesystem::path newVersionPath = versionsPath / versionFolder;

        if (!std::filesystem::exists(repoPath)) {
            #ifdef _WIN32
                std::cout << '\a';
            #endif
            return false;
        }
        auto ignoreRules = ViciIgnore::parse(repoPath);
        copyDirectory(repoPath, newVersionPath, ignoreRules);

        if (!message.empty()) {
            std::ofstream msgFile(newVersionPath / "commit_message.txt");
            msgFile << message;
            msgFile.close();
        }
        return true;
    } catch (...) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        return false;
    }
}

std::vector<std::string> VersionControl::listVersions(const std::string& repoName) {
    std::vector<std::string> versions;
    auto versionsPath = getVersionsPath(repoName);
    if (std::filesystem::exists(versionsPath) && std::filesystem::is_directory(versionsPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(versionsPath)) {
            if (entry.is_directory()) {
                versions.push_back(entry.path().filename().string());
            }
        }
    }
    std::sort(versions.begin(), versions.end(), std::greater<std::string>());
    return versions;
}

bool VersionControl::checkout(const std::string& repoName, const std::string& version) {
    try {
        std::filesystem::path repoPath = getRepoPath(repoName);
        std::filesystem::path versionsPath = getVersionsPath(repoName);
        std::filesystem::path versionPath = versionsPath / version;

        if (!std::filesystem::exists(versionPath)) {
            #ifdef _WIN32
                std::cout << '\a';
            #endif
            return false;
        }

        auto ignoreRules = ViciIgnore::parse(repoPath);
        copyDirectory(versionPath, repoPath, ignoreRules);
        return true;
    } catch (...) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        return false;
    }
}

bool VersionControl::deleteRepo(const std::string& repoName) {
    try {
        auto repoPath = getRepoPath(repoName);
        auto versionsPath = getVersionsPath(repoName);
        std::filesystem::remove_all(repoPath);
        std::filesystem::remove_all(versionsPath);
        return true;
    } catch (...) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        return false;
    }
}

bool VersionControl::deleteVersion(const std::string& repoName, const std::string& version) {
    try {
        auto versionPath = getVersionsPath(repoName) / version;
        std::filesystem::remove_all(versionPath);
        return true;
    } catch (...) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
        return false;
    }
}

std::string VersionControl::getLatestVersion(const std::string& repoName) {
    auto versions = listVersions(repoName);
    if (!versions.empty()) {
        return versions.front();
    }
    #ifdef _WIN32
        std::cout << '\a';
    #endif
    return {};
}

std::string VersionControl::getCommitMessage(const std::string& repoName, const std::string& version) {
    std::string message;
    try {
        auto msgFile = getVersionsPath(repoName) / version / "commit_message.txt";
        if (std::filesystem::exists(msgFile)) {
            std::ifstream file(msgFile);
            std::getline(file, message);
        } else {
            #ifdef _WIN32
                std::cout << '\a';
            #endif
        }
    } catch (...) {
        #ifdef _WIN32
            std::cout << '\a';
        #endif
    }
    return message;
}