#include <filesystem>
#include <algorithm>
#include <cstdio>

#include "util.h"
#include "loader.h"
#include "reconstruct.h"

Loader loader;

namespace fs = std::filesystem;

void print_usage(char *argv[]) {
    fprintf(stderr, "Usage: %s path [assigned count]\n", argv[0]);
    exit(0);
}

int main(int argc, char *argv[]) {
    /*
     * Arguments:
     * 0: executable name
     * 1. path (file or directory)
     * [2. assigned part]
     * [3. number of parts]
     */

    if (argc != 2 && argc != 4) {
        print_usage(argv);
    }

    int c = 0;

    std::string folderPath(argv[1]);

    if (fs::is_directory(folderPath)) {
        std::vector<std::string> fileNames;
        for (const auto &entry : fs::directory_iterator(folderPath)) {
            fileNames.push_back(entry.path());
        }

        std::sort(fileNames.begin(), fileNames.end());

        int N = fileNames.size();

        int start_index;
        int end_index;

        if (argc == 4) {
            int assignedPart = atoi(argv[2]);
            int numParts = atoi(argv[3]);

            start_index = (N * assignedPart) / numParts;
            end_index = (N * (assignedPart + 1)) / numParts;

            LOG("Processing images #%d to #%d\n", start_index, end_index);
        } else {
            start_index = 0;
            end_index = N;

            LOG("Processing %d images\n", N);
        }


        for (int i = start_index; i < end_index; ++i) {
            std::string name = fileNames[i];
            // std::cout << entry.path() << std::endl;
            // reconstructImage(entry.path());
            unshuffleImage(name).write(stdout, true); // prune paths if input is a directory
            ++c;
        }
    } else {
        if (argc != 2) {
            print_usage(argv);
        }

        if (!fs::exists(folderPath)) {
            fprintf(stderr, "File %s does not exist\n", folderPath.c_str());
            exit(2);
        }

        unshuffleImage(folderPath).write(stdout, false);
    }

    return 0;
}

