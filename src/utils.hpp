#pragma once

class InitUtils{
    public:
        static void listDirectoryContents(const std::filesystem::path& path);
        static void loadRepos();
};

class loopUtils{
    public:
        static void initLoop();
};