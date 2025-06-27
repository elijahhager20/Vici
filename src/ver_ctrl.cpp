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
            if (relStr == rule.pattern || relStr.rfind(rule.pattern + "/", 0) == 0) return isDir;
        } else if (rule.type == IgnoreRule::Type::FolderFiles) {
            if (!isDir && relStr.rfind(rule.pattern + "/", 0) == 0 && relPath.parent_path().generic_string() == rule.pattern)
                return true;
        }
    }
    return false;
}

std::filesystem::path VersionControl::getRepoPath(const std::string& repoName) {
    return std::filesystem::path("user_repos") / (repoName + ".curr");
}

std::filesystem::path VersionControl::getVersionsPath(const std::string& repoName) {
    return std::filesystem::path("user_repos") / repoName;
}

bool VersionControl::repoExists(const std::string& repoName) {
    auto repoPath = getRepoPath(repoName);
    return std::filesystem::exists(repoPath) && std::filesystem::is_directory(repoPath);
}

bool VersionControl::versionsFolderExists(const std::string& repoName) {
    auto versionsPath = getVersionsPath(repoName);
    return std::filesystem::exists(versionsPath) && std::filesystem::is_directory(versionsPath);
}

bool VersionControl::checkRepoSelected(const std::string& currentRepo) {
    if (currentRepo.empty()) {
        std::cout << "Error: No repository selected.\n";
        return false;
    }
    return true;
}

bool VersionControl::checkArgsSize(const std::vector<std::string>& args, size_t minSize, const std::string& usage) {
    if (args.size() < minSize) {
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
        return true;
    }
    std::cout << "Error: Repository '" << repoName << "' does not exist.\n";
    return false;
}

bool VersionControl::createRepo(const std::vector<std::string>& args) {
    if (!checkArgsSize(args, 2, "new <repo_name>")) return false;
    std::string repoName = args[1];
    auto repoPath = getRepoPath(repoName);
    auto versionsPath = getVersionsPath(repoName);
    if (repoExists(repoName)) {
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
        std::cout << "Error: Failed to create repository: " << e.what() << "\n";
        return false;
    }
}

bool VersionControl::commitRepo(const std::vector<std::string>& args, const std::string& currentRepo, const std::string& in) {
    if (!checkRepoSelected(currentRepo)) return false;
    std::string message;
    if (args.size() > 1) {
        message = in.substr(in.find(args[1]));
    }
    if (commit(currentRepo, message)) {
        std::cout << "Committed current state of '" << currentRepo << "'.\n";
        return true;
    }
    std::cout << "Error: Commit failed.\n";
    return false;
}

bool VersionControl::logRepo(const std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    auto versions = listVersions(currentRepo);
    if (versions.empty()) {
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

bool VersionControl::checkoutRepo(const std::vector<std::string>& args, const std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    if (!checkArgsSize(args, 2, "checkout <version>")) return false;
    if (checkout(currentRepo, args[1])) {
        std::cout << "Checked out version " << args[1] << " to '" << currentRepo << ".curr'.\n";
        return true;
    }
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
    std::cout << "Error: Failed to delete repository.\n";
    return false;
}

bool VersionControl::deleteVersionCmd(const std::vector<std::string>& args, const std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    if (!checkArgsSize(args, 2, "delver <version>")) return false;
    if (deleteVersion(currentRepo, args[1])) {
        std::cout << "Version '" << args[1] << "' deleted.\n";
        return true;
    }
    std::cout << "Error: Failed to delete version.\n";
    return false;
}

bool VersionControl::statusRepo(const std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    std::cout << "Current repo: " << currentRepo << "\n";
    std::string latest = getLatestVersion(currentRepo);
    if (!latest.empty()) {
        std::cout << "Latest commit: " << latest << "\n";
    } else {
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
        "  help                    Show this help message\n"
        "  exit                    Exit the program\n";
}

bool VersionControl::listFiles(const std::string& currentRepo) {
    if (!checkRepoSelected(currentRepo)) return false;
    auto repoPath = getRepoPath(currentRepo);
    if (!repoExists(currentRepo)) {
        std::cout << "Error: Current repo folder does not exist.\n";
        return false;
    }
    std::cout << "Files in " << repoPath.string() << ":\n";
    InitUtils::listDirectoryContents(repoPath);
    return true;
}

void VersionControl::copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination, const std::vector<IgnoreRule>& ignoreRules) {
    std::filesystem::create_directories(destination);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(source)) {
        auto relPath = std::filesystem::relative(entry.path(), source);
        bool isDir = entry.is_directory();
        if (ViciIgnore::isIgnored(ignoreRules, relPath, isDir) || relPath.filename() == ".viciignore") continue;
        auto destPath = destination / relPath;
        if (isDir) {
            std::filesystem::create_directories(destPath);
        } else if (entry.is_regular_file()) {
            std::filesystem::copy_file(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
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
        std::sort(versions.begin(), versions.end());
    }
    return versions;
}

bool VersionControl::checkout(const std::string& repoName, const std::string& version) {
    auto versionsPath = getVersionsPath(repoName) / version;
    auto repoPath = getRepoPath(repoName);
    if (!std::filesystem::exists(versionsPath) || !std::filesystem::is_directory(versionsPath)) {
        return false;
    }
    if (std::filesystem::exists(repoPath)) {
        std::filesystem::remove_all(repoPath);
    }
    copyDirectory(versionsPath, repoPath);
    return true;
}

bool VersionControl::deleteRepo(const std::string& repoName) {
    auto repoPath = getRepoPath(repoName);
    auto versionsPath = getVersionsPath(repoName);
    bool ok = true;
    if (std::filesystem::exists(repoPath)) {
        ok &= std::filesystem::remove_all(repoPath) > 0;
    }
    if (std::filesystem::exists(versionsPath)) {
        ok &= std::filesystem::remove_all(versionsPath) > 0;
    }
    return ok;
}

bool VersionControl::deleteVersion(const std::string& repoName, const std::string& version) {
    auto versionPath = getVersionsPath(repoName) / version;
    if (std::filesystem::exists(versionPath)) {
        return std::filesystem::remove_all(versionPath) > 0;
    }
    return false;
}

std::string VersionControl::getLatestVersion(const std::string& repoName) {
    auto versions = listVersions(repoName);
    if (!versions.empty()) {
        return versions.back();
    }
    return "";
}

std::string VersionControl::getCommitMessage(const std::string& repoName, const std::string& version) {
    auto msgPath = getVersionsPath(repoName) / version / "commit_message.txt";
    if (std::filesystem::exists(msgPath)) {
        std::ifstream in(msgPath);
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }
    return "";
}