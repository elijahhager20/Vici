#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "ver_ctrl.hpp"

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

namespace {
    std::string currentRepo = "";
}

std::string& loopUtils::getCurrentRepo() {
    return currentRepo;
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
    auto& currentRepo = getCurrentRepo();
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
            std::filesystem::path repoPath = std::filesystem::path("user_repos") / (repoName + ".curr");
            if (std::filesystem::exists(repoPath) && std::filesystem::is_directory(repoPath)) {
                currentRepo = repoName;
                std::cout << "Switched to repository: " << repoName << "\n";
            } else {
                std::cout << "Repository '" << repoName << "' does not exist.\n";
            }
        }
        return true;
    }
    if (args[0] == "-new") {
        if (args.size() < 2) {
            std::cout << "Usage: -new <repo_name>\n";
        } else {
            std::string repoName = args[1];
            std::filesystem::path repoPath = std::filesystem::path("user_repos") / (repoName + ".curr");
            std::filesystem::path versionsPath = std::filesystem::path("user_repos") / repoName;
            if (std::filesystem::exists(repoPath)) {
                std::cout << "Repository '" << repoName << "' already exists.\n";
            } else {
                try {
                    std::filesystem::create_directories(repoPath);
                    std::filesystem::create_directories(versionsPath);
                    std::ofstream initFile(repoPath / "README.txt");
                    initFile << "Repository: " << repoName << "\nCreated with Vici.\n";
                    initFile.close();
                    std::cout << "Repository '" << repoName << "' created successfully.\n";
                    std::cout << "Versions folder '" << versionsPath.string() << "' created successfully.\n";
                } catch (const std::exception& e) {
                    std::cout << "Failed to create repository: " << e.what() << "\n";
                }
            }
        }
        return true;
    }
    if (args[0] == "-commit") {
        if (currentRepo.empty()) {
            std::cout << "No repository selected. Use -cr <repo_name> to select a repository.\n";
        } else {
            if (VersionControl::commit(currentRepo)) {
                std::cout << "Committed current state of '" << currentRepo << "'.\n";
            } else {
                std::cout << "Commit failed.\n";
            }
        }
        return true;
    }

    std::cout << "Unknown command: " << args[0] << "\n";
    return true;
}