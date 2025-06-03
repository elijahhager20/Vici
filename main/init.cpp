#include "init.hpp"
#include <iostream>
#include <filesystem>

void Initialization::mainInit(){
    #ifdef FTI
    #else
        firstTimeInit();
    #endif
    }
    
void Initialization::firstTimeInit(){
    std::string userInput;
    
    std::cout << "Welcome to the Vici version control system! What would you like to do?\nInfo\nCreate Repository\nExit\n";
    std::cin  >> userInput;

    if (userInput == "info" || userInput == "Info"){
        std::cout << '\n';
    } else if (userInput == "Create Repository" || userInput == "create repository" || userInput == "create Repository" || userInput == "Create repository"){
        std::cout << '\n';
    }
}

void createRepos::loadRepos(){
    
}

void createRepos::createFirstRepo(){
    std::cout << "What would you like to name your first repository?: ";
}
