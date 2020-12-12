#pragma once

#include <set>
#include <string>
#include <vector>

#include <cassert>
#include <cstdio>
#include <cstring>

#include "settings.h"
#include "best.h"
#include "util.h"
#include "grid.h"
#include "estimate.h"

class Reconstruction {
    const std::vector<int> perm;
    const std::optional<std::string> source;

    Reconstruction(std::vector<int> permutation, std::optional<std::string> source)
        : perm(permutation), source(source) {
        for (auto x : perm) {
            assert(x >= 0);
        }
    }

public:

    Reconstruction(std::vector<int> permutation, std::string source)
        : Reconstruction(permutation, std::optional(source)) {}
    Reconstruction(std::vector<int> permutation) : Reconstruction(permutation, std::nullopt) {}

    void write(std::string path, FILE *fp, bool prunePath = false);
    void write(FILE *fp, bool prunePath = false);
};

Reconstruction unshuffleImage(std::string path);

