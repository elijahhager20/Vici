/*
Vici, the worst
version control system
known to man
*/

#include "utils.hpp"
#include "ver_ctrl.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

int main(int argc, char* argv[]) {
    auto userReposPath = getBaseDir() / "user_repos";
    if (!std::filesystem::exists(userReposPath)) {
        std::filesystem::create_directory(userReposPath);
    }

    if (argc > 1) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) args.push_back(argv[i]);
        std::string currentRepo;
        std::string in;
        for (int i = 1; i < argc; ++i) {
            if (i > 1) in += " ";
            in += argv[i];
        }

        if (args[0] == "cr") VersionControl::selectRepo(args, currentRepo);
        else if (args[0] == "new") VersionControl::createRepo(args);
        else if (args[0] == "commit") VersionControl::commitRepo(args, currentRepo, in);
        else if (args[0] == "log") VersionControl::logRepo(currentRepo);
        else if (args[0] == "checkout") VersionControl::checkoutRepo(args, currentRepo);
        else if (args[0] == "delrepo") VersionControl::deleteRepoCmd(args, currentRepo);
        else if (args[0] == "delver") VersionControl::deleteVersionCmd(args, currentRepo);
        else if (args[0] == "status") VersionControl::statusRepo(currentRepo);
        else if (args[0] == "help") VersionControl::help();
        else if (args[0] == "ls") VersionControl::listFiles(currentRepo);
        else if (args[0] == "cls") {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
        } else {
            #ifdef _WIN32
                std::cout << '\a';
            #endif
            std::cout << "Unknown command: " << args[0] << "\n";
            return 1;
        }
        return 0;
    } else {
        LoopUtils::Loop();
        std::ofstream(getBaseDir() / "user_repos" / ".vicicurr", std::ios::trunc).close();
        return 0;
    }
}

/*
you_suck7, elijahhager20, hawkpier30,
trollface., playbloxRB, & 
txap were here
*/