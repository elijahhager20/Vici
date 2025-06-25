#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "ver_ctrl.hpp"
#include <cstdlib>

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
        std::cout << '@' << currentRepo << ": ";
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
    if (args[0] == "cr") {
        if (args.size() < 2) {
            std::cout << "Usage: cr <repo_name>\n";
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
    if (args[0] == "new") {
        if (args.size() < 2) {
            std::cout << "Usage: new <repo_name>\n";
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
    if (args[0] == "commit") {
        if (currentRepo.empty()) {
            std::cout << "No repository selected. Use cr <repo_name> to select a repository.\n";
        } else {
            std::string message;
            if (args.size() > 1) {
                message = in.substr(in.find(args[1]));
            }
            if (VersionControl::commit(currentRepo, message)) {
                std::cout << "Committed current state of '" << currentRepo << "'.\n";
            } else {
                std::cout << "Commit failed.\n";
            }
        }
        return true;
    }
    if (args[0] == "log") {
        if (currentRepo.empty()) {
            std::cout << "No repository selected.\n";
        } else {
            auto versions = VersionControl::listVersions(currentRepo);
            if (versions.empty()) {
                std::cout << "No commits found.\n";
            } else {
                std::cout << "Commits for '" << currentRepo << "':\n";
                for (const auto& v : versions) {
                    std::cout << v;
                    std::string msg = VersionControl::getCommitMessage(currentRepo, v);
                    if (!msg.empty()) std::cout << " - " << msg;
                    std::cout << "\n";
                }
            }
        }
        return true;
    }
    if (args[0] == "checkout") {
        if (currentRepo.empty()) {
            std::cout << "No repository selected.\n";
        } else if (args.size() < 2) {
            std::cout << "Usage: checkout <version>\n";
        } else {
            if (VersionControl::checkout(currentRepo, args[1])) {
                std::cout << "Checked out version " << args[1] << " to '" << currentRepo << ".curr'.\n";
            } else {
                std::cout << "Checkout failed.\n";
            }
        }
        return true;
    }
    if (args[0] == "delrepo") {
        if (args.size() < 2) {
            std::cout << "Usage: delrepo <repo_name>\n";
        } else {
            if (VersionControl::deleteRepo(args[1])) {
                std::cout << "Repository '" << args[1] << "' deleted.\n";
                if (currentRepo == args[1]) currentRepo.clear();
            } else {
                std::cout << "Failed to delete repository.\n";
            }
        }
        return true;
    }
    if (args[0] == "delver") {
        if (currentRepo.empty()) {
            std::cout << "No repository selected.\n";
        } else if (args.size() < 2) {
            std::cout << "Usage: delver <version>\n";
        } else {
            if (VersionControl::deleteVersion(currentRepo, args[1])) {
                std::cout << "Version '" << args[1] << "' deleted.\n";
            } else {
                std::cout << "Failed to delete version.\n";
            }
        }
        return true;
    }
    if (args[0] == "status") {
        if (currentRepo.empty()) {
            std::cout << "No repository selected.\n";
        } else {
            std::cout << "Current repo: " << currentRepo << "\n";
            std::string latest = VersionControl::getLatestVersion(currentRepo);
            if (!latest.empty()) {
                std::cout << "Latest commit: " << latest << "\n";
            } else {
                std::cout << "No commits yet.\n";
            }
        }
        return true;
    }
    if (args[0] == "help") {
        std::cout <<
            "Commands:\n"
            "  new <repo_name>         Create a new repository\n"
            "  cr <repo_name>          Change/select repository\n"
            "  commit [msg]            Commit current state with optional message\n"
            "  log                     List all commits for current repo\n"
            "  checkout <version>      Restore repo to a previous version\n"
            "  delrepo <repo_name>     Delete a repository and its versions\n"
            "  delver <version>        Delete a specific version from current repo\n"
            "  status                  Show current repo and latest commit\n"
            "  ls                      List files in the current repo's .curr folder\n"
            "  help                    Show this help message\n"
            "  exit                    Exit the program\n";
        return true;
    }
    if (args[0] == "ls") {
        if (currentRepo.empty()) {
            std::cout << "No repository selected.\n";
        } else {
            std::filesystem::path repoPath = std::filesystem::path("user_repos") / (currentRepo + ".curr");
            if (!std::filesystem::exists(repoPath) || !std::filesystem::is_directory(repoPath)) {
                std::cout << "Current repo folder does not exist.\n";
            } else {
                std::cout << "Files in " << repoPath.string() << ":\n";
                InitUtils::listDirectoryContents(repoPath);
            }
        }
        return true;
    }
    if (args[0] == "cls"){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    return true;
}

    std::cout << "Unknown command: " << args[0] << "\n";
    return true;
}