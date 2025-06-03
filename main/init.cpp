#include "init.hpp"
#include <iostream>

void Initialization::mainInit(){
    #ifdef FTI
    #else
        firstTimeInit();
    #endif
    }
    
void Initialization::firstTimeInit(){
    std::string userInput;
    
    std::cout << "Welcome to the Vici version control system! What would you like to do?\nLicsense\nCreate Repository\nExit\n";
    std::cin  >> userInput;

    if (userInput == "info" || userInput == "Info"){
        std::cout << '\n';
    }
}

void createRepos::loadRepos(){

}

void createRepos::createFirstRepo(){

}
