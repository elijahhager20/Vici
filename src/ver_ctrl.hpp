#pragma once
#include <string>

class VersionControl {
public:
    static bool commit(const std::string& repoName);
};