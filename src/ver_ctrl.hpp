#pragma once
#include <string>
#include <vector>

class VersionControl {
public:
    static bool commit(const std::string& repoName, const std::string& message = "");
    static std::vector<std::string> listVersions(const std::string& repoName);
    static bool checkout(const std::string& repoName, const std::string& version);
    static bool deleteRepo(const std::string& repoName);
    static bool deleteVersion(const std::string& repoName, const std::string& version);
    static std::string getLatestVersion(const std::string& repoName);
    static std::string getCommitMessage(const std::string& repoName, const std::string& version);
};