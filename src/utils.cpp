#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>
#include "utils.hpp"

// Helper to split input into arguments
std::vector<std::string> splitArgs(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }
    return args;
}

void InitUtils::listDirectoryContents(const std::filesystem::path& path){
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const auto& name = entry.path().filename().string();
        if (name != "." && name != "..") {
            std::cout << name << '\n';
        }
    }
}

void InitUtils::loadRepos(){
    std::filesystem::path repoName;
    std::filesystem::path currRepo;
    std::string repoNameStr;

    std::cout << "Which repo would you like to load?: ";
    std::getline(std::cin, repoNameStr);
    repoName = repoNameStr + ".curr";
    currRepo = repoName;

    InitUtils::listDirectoryContents("user_repos"/currRepo);
}

void loopUtils::Loop(){
    std::string currIn;
    while(true){
        std::cout << "@viciw.exe: ";
        std::getline(std::cin, currIn);

        if (!loopUtils::logic(currIn)) {
            break;
        }
    }
}

bool loopUtils::logic(std::string& in){
    auto args = splitArgs(in);
    if (args.empty()) return true;

    if (args[0] == "exit") {
        return false;
    }
    if (args[0] == "-cr") {
        if (args.size() < 2) {
            std::cout << "Usage: -cr <repo_name>\n";
        } else {
            std::string repoName = args[1];
            std::cout << "Switching to repository: " << repoName << "\n";
        }
        return true;
    }

    std::cout << "Unknown command: " << args[0] << "\n";
    return true;
}