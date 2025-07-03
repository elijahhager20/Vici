#pragma once
#include <string>
#include <filesystem>
#include <vector>

class Utils {
public:
    static std::filesystem::path getBaseDir();
    static std::vector<std::string> splitArgs(const std::string& input);
};

class InitUtils {
public:
    static void listDirectoryContents(const std::filesystem::path& path);
};

class LoopUtils {
public:
    static void loop();
    static bool logic(std::string& in);
    static std::string& getCurrentRepo();
};