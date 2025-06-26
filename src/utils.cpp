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

    if (args[0] == "exit") return false;
    if (args[0] == "cr") return VersionControl::selectRepo(args, currentRepo);
    if (args[0] == "new") return VersionControl::createRepo(args);
    if (args[0] == "commit") return VersionControl::commitRepo(args, currentRepo, in);
    if (args[0] == "log") return VersionControl::logRepo(currentRepo);
    if (args[0] == "checkout") return VersionControl::checkoutRepo(args, currentRepo);
    if (args[0] == "delrepo") return VersionControl::deleteRepoCmd(args, currentRepo);
    if (args[0] == "delver") return VersionControl::deleteVersionCmd(args, currentRepo);
    if (args[0] == "status") return VersionControl::statusRepo(currentRepo);
    if (args[0] == "help"){VersionControl::help(); return true;}
    if (args[0] == "ls") return VersionControl::listFiles(currentRepo);
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