/*
Javascript warrior at day

C++ god at night
*/

#include "init.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include "ver_ctrl.hpp"
#include "utils.hpp"

void Init::mainInit(){
    if (!std::filesystem::exists("sys_info")){
        Init::firstTimeInit();
    } else{
        createRepos::loadRepos();
    }
}
    
void Init::firstTimeInit(){
    std::string userInput;
    
    std::cout << "Welcome to the Vici version control system! What would you like to do?\nInfo\nCreate Repository\nExit\n";
    std::getline(std::cin, userInput);

    if (userInput == "info" || userInput == "Info"){
        std::cout << '\n';
    } else if (userInput == "Create Repository" || userInput == "create repository" || userInput == "create Repository" || userInput == "Create repository"){
        std::cout << '\n';
        createRepos::createFirstRepo();
    }
}

void createRepos::loadRepos(){
    std::filesystem::path repoName;
    std::filesystem::path currRepo;
    std::string repoNameStr;

    std::cout << "Which repo would you like to load?: ";
    std::getline(std::cin, repoNameStr);
    repoName = repoNameStr + ".curr";
    currRepo = repoName;

    InitUtils::listDirectoryContents("user_repos"/currRepo);
}

void createRepos::createFirstRepo(){
    std::ofstream FTUF("sys_info");
    FTUF.close();

    std::string newRepoNameStr;
    std::cout << "What would you like to name your first repository?: ";
    std::getline(std::cin, newRepoNameStr);

    std::filesystem::path base = "user_repos";
    std::filesystem::path currPath = base / (newRepoNameStr + ".curr");
    std::filesystem::path oldPath = base / newRepoNameStr;

    if (!std::filesystem::exists(currPath)) {
        if (std::filesystem::create_directory(currPath)) {
            std::cout << "Folder '" << currPath << "' created successfully\n";
        } else {
            std::cerr << "Failed to create folder '" << currPath << "'\n";
        }
    }

    if (!std::filesystem::exists(oldPath)) {
        if (std::filesystem::create_directory(oldPath)) {
            std::cout << "Folder '" << oldPath << "' created successfully\n";
        } else {
            std::cerr << "Failed to create folder '" << oldPath << "'\n";
        }
    }
}