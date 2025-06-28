#include "utils.hpp"
#include <filesystem>
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

namespace {
    std::string currentRepo = "";
}

std::string& LoopUtils::getCurrentRepo() {
    return currentRepo;
}

void LoopUtils::Loop(){
    std::string currIn;
    std::string lastCmd;
    while(true){
        std::cout << '@' << currentRepo << ": ";
        std::getline(std::cin, currIn);
        if (currIn == "!!") {
            if (lastCmd.empty()) {
#ifdef _WIN32
                std::cout << '\a';
#endif
                std::cout << "No previous command.\n";
                continue;
            }
            currIn = lastCmd;
            std::cout << currIn << std::endl;
        } else if (!currIn.empty()) {
            lastCmd = currIn;
        }
        if (!LoopUtils::logic(currIn)) {
            break;
        }
    }
}

bool LoopUtils::logic(std::string& in){
    auto& currentRepo = getCurrentRepo();
    auto args = splitArgs(in);
    if (args.empty()) {
#ifdef _WIN32
        std::cout << '\a';
#endif
        return true;
    }

    if (args[0] == "exit") return false;
    if (args[0] == "cr") { VersionControl::selectRepo(args, currentRepo); return true; }
    if (args[0] == "new") { VersionControl::createRepo(args); return true; }
    if (args[0] == "commit") { VersionControl::commitRepo(args, currentRepo, in); return true; }
    if (args[0] == "log") { VersionControl::logRepo(currentRepo); return true; }
    if (args[0] == "checkout") { VersionControl::checkoutRepo(args, currentRepo); return true; }
    if (args[0] == "delrepo") { VersionControl::deleteRepoCmd(args, currentRepo); return true; }
    if (args[0] == "delver") { VersionControl::deleteVersionCmd(args, currentRepo); return true; }
    if (args[0] == "status") { VersionControl::statusRepo(currentRepo); return true; }
    if (args[0] == "help"){ VersionControl::help(); return true; }
    if (args[0] == "ls") { VersionControl::listFiles(currentRepo); return true; }
    if (args[0] == "cls"){
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        return true;
    }
#ifdef _WIN32
    std::cout << '\a';
#endif
    std::cout << "Unknown command: " << args[0] << "\n";
    return true;
}