#pragma once
#include <string>
#include <filesystem>
#include <vector>

class InitUtils {
public:
    static void listDirectoryContents(const std::filesystem::path& path);
    static void loadRepos();
};

class loopUtils {
public:
    static void Loop();
    static bool logic(std::string& in);
    static std::string& getCurrentRepo();
};

std::vector<std::string> splitArgs(const std::string& input);