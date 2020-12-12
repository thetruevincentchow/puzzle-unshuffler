#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "settings.h"
#include "loader.h"
#include "best.h"
#include "util.h"
#include "grid.h"

using PieceImage = float[PIECE_SIZE][PIECE_SIZE][3];
using Dissimilarity = float[NUM_PIECES][NUM_PIECES][4];
using Pieces = PieceImage[PER_SIDE][PER_SIDE];

struct Placement {
    int piece;
    Pos pos;

    Placement(int piece, Pos pos) : piece(piece), pos(pos) { }
};

class Estimate {
    Image image;
    Pieces pieces;
    Dissimilarity dis;

    float C[NUM_PIECES][NUM_PIECES][4]; // compatibility function
    Best<int, float> best[NUM_PIECES][4];
    float bestC[NUM_PIECES][4];

public:
    std::vector<int> result;

    Estimate(std::string path) {
        //LOG("Load %s\n", path.c_str());

        loader.loadImage(path, image);
        separatePieces();
        computeDissimilarity(dis); // This only needs to be done once.

        initBest();
        // however, the compatibility function may change

        int seed = computeBestBuddies();
        if (seed < 0) {
            fprintf(stderr, "Failed %s\n", path.c_str());
        } else {
            result = greedyPlacement(seed);
        }
    }

    void separatePieces(void);
    void computeDissimilarity(Dissimilarity &out);
    void initBest(void);
    bool isBestBuddies(int a, int b, int d);
    float mutualScore(int a, int b, int d);
    void updateC(void);
    int computeBestBuddies(void);

    std::vector<int> greedyPlacement(int seed);
};


