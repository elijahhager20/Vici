#include "ver_ctrl.hpp"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace {
    void copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination) {
        std::filesystem::create_directories(destination);
        for (const auto& entry : std::filesystem::recursive_directory_iterator(source)) {
            const auto& relPath = std::filesystem::relative(entry.path(), source);
            const auto& destPath = destination / relPath;
            if (entry.is_directory()) {
                std::filesystem::create_directories(destPath);
            } else if (entry.is_regular_file()) {
                std::filesystem::copy_file(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
            }
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

        std::filesystem::path repoPath = std::filesystem::path("user_repos") / (repoName + ".curr");
        std::filesystem::path versionsPath = std::filesystem::path("user_repos") / repoName;
        std::filesystem::path newVersionPath = versionsPath / versionFolder;

        if (!std::filesystem::exists(repoPath)) {
            return false;
        }
        copyDirectory(repoPath, newVersionPath);

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
    std::filesystem::path versionsPath = std::filesystem::path("user_repos") / repoName;
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
    std::filesystem::path versionsPath = std::filesystem::path("user_repos") / repoName / version;
    std::filesystem::path repoPath = std::filesystem::path("user_repos") / (repoName + ".curr");
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
    std::filesystem::path repoPath = std::filesystem::path("user_repos") / (repoName + ".curr");
    std::filesystem::path versionsPath = std::filesystem::path("user_repos") / repoName;
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
    std::filesystem::path versionPath = std::filesystem::path("user_repos") / repoName / version;
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
    std::filesystem::path msgPath = std::filesystem::path("user_repos") / repoName / version / "commit_message.txt";
    if (std::filesystem::exists(msgPath)) {
        std::ifstream in(msgPath);
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }
    return "";
}