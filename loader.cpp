#include <cstdio>
#include <cstring>
#include <string>
#include <cassert>

#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "settings.h"
#include "loader.h"

/*
 * Creates a child process to load image data.
 */
void Loader::setupFork(void) {
    pipe(fd_read);
    pipe(fd_write);
    childPID = fork();

    if (childPID == 0) { // child process
        dup2(fd_read[0], fileno(stdin));
        dup2(fd_write[1], fileno(stdout));

        close(fd_read[0]);
        close(fd_write[1]);

        char *const argv[] = {strdup("python3"), strdup("loader.py"), NULL};
        execvp("python3", argv);
    } else { // parent process
        close(fd_read[0]);
        close(fd_write[1]);
    }
}

void Loader::endFork(void) {
    close(fd_read[1]);
    close(fd_write[0]);

    kill(childPID, SIGTERM);
    waitpid(childPID, NULL, 0);
}

Loader::Loader() {
    setupFork();
}

Loader::~Loader() {
    endFork();
}

void Loader::loadImage(std::string path, Image &image) {
    dprintf(fd_read[1], "%s\n", path.c_str());
    unsigned char buf[IMAGE_SIZE * IMAGE_SIZE * 3];

    fprintf(stderr,"Loading image %s\n",path.c_str());

    ssize_t expected_size = IMAGE_SIZE * IMAGE_SIZE * 3;
    ssize_t s = 0;
    while (s < expected_size) {
        ssize_t read_bytes = read(fd_write[0], buf + s, expected_size);
        if (read_bytes < 0) {
            fprintf(stderr, "I/O error\n");
            exit(2);
        }
        s += read_bytes;
    }

    fprintf(stderr,"Got %zu bytes, expected %zu bytes\n", s, expected_size);
    if (s != expected_size) {
        exit(1);
    }

    int c = 0;
    for (int i = 0; i < IMAGE_SIZE; ++i) {
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            for (int k = 0; k < 3; ++k) {
                float w = buf[c++];
                assert(0. <= w && w <= 255.);
                image[i][j][k] = w;
            }
        }
    }
}
