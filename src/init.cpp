#include "init.hpp"
#include "ver_ctrl.hpp"
#include "utils.hpp"
#include <iostream>
#include <filesystem>

void Init::mainInit() {
    auto userReposPath = getBaseDir() / "user_repos";
    if (!std::filesystem::exists(userReposPath)) {
        std::filesystem::create_directory(userReposPath);
    }
    LoopUtils::Loop();
}