#include <filesystem>
#include <iostream>
#include "utils.hpp"

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

void loopUtils::initLoop(){
    
}