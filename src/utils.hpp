#pragma once

class InitUtils{
    public:
        static void listDirectoryContents(const std::filesystem::path& path);
        static void loadRepos();
};