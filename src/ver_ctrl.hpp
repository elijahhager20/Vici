#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <set>

struct IgnoreRule {
    enum class Type { File, Folder, FolderFiles };
    std::string pattern;
    Type type;
};

class ViciIgnore {
public:
    static std::vector<IgnoreRule> parse(const std::filesystem::path& repoPath);
    static bool isIgnored(const std::vector<IgnoreRule>& rules, const std::filesystem::path& relPath, bool isDir);
};

class VersionControl {
public:
    static std::filesystem::path getRepoPath(const std::string& repoName);
    static std::filesystem::path getVersionsPath(const std::string& repoName);
    static bool repoExists(const std::string& repoName);
    static bool versionsFolderExists(const std::string& repoName);

    static bool selectRepo(const std::vector<std::string>& args, std::string& currentRepo);
    static bool createRepo(const std::vector<std::string>& args);
    static bool commitRepo(const std::vector<std::string>& args, std::string& currentRepo, const std::string& in);
    static bool logRepo(std::string& currentRepo);
    static bool checkoutRepo(const std::vector<std::string>& args, std::string& currentRepo);
    static bool deleteRepoCmd(const std::vector<std::string>& args, std::string& currentRepo);
    static bool deleteVersionCmd(const std::vector<std::string>& args, std::string& currentRepo);
    static bool statusRepo(std::string& currentRepo);
    static void help();
    static bool listFiles(std::string& currentRepo);

    static bool commit(const std::string& repoName, const std::string& message = "");
    static std::vector<std::string> listVersions(const std::string& repoName);
    static bool checkout(const std::string& repoName, const std::string& version);
    static bool deleteRepo(const std::string& repoName);
    static bool deleteVersion(const std::string& repoName, const std::string& version);
    static std::string getLatestVersion(const std::string& repoName);
    static std::string getCommitMessage(const std::string& repoName, const std::string& version);

private:
    static bool checkRepoSelected(std::string& currentRepo);
    static bool checkArgsSize(const std::vector<std::string>& args, size_t minSize, const std::string& usage);
    static void copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination, const std::vector<IgnoreRule>& ignoreRules = {});
};