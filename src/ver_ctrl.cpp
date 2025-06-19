#include "ver_ctrl.hpp"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

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

bool VersionControl::commit(const std::string& repoName) {
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
        return true;
    } catch (...) {
        return false;
    }
}