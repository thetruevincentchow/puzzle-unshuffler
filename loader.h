#pragma once
#include "settings.h"

class Loader {
    int fd_read[2], fd_write[2], childPID;

    void setupFork(void);
    void endFork(void);
public:
    Loader();
    ~Loader();

    void loadImage(std::string path, Image &image);
};

extern Loader loader;
