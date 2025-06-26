#include "init.hpp"
#include "ver_ctrl.hpp"
#include "utils.hpp"
#include <iostream>
#include <filesystem>

void Init::mainInit() {
    if (!std::filesystem::exists("user_repos")) {
        std::filesystem::create_directory("user_repos");
    }
    loopUtils::Loop();
}