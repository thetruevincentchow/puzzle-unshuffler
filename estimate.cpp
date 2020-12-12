#include "estimate.h"

constexpr int moore[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

/*
 * Bitfields are used to represent relative positions.
 */
inline int opp(int cur) {
    return cur ^ 1;
}

inline int perp(int cur) {
    return cur ^ 2;
}


/*
 * Cut the image up into square puzzle pieces
 */
void Estimate::separatePieces(void) {
    for (auto [ax, ay, a] : Array2DIterator(PER_SIDE, PER_SIDE)) {
        PieceImage &piece = pieces[ax][ay];
        for(auto [bx, by, b] : Array2DIterator(PIECE_SIZE, PIECE_SIZE)) {
            for (int k = 0; k < 3; ++k) {
                piece[bx][by][k] = image[ax * PIECE_SIZE + bx]
                                        [ay * PIECE_SIZE + by][k];
            }
        }
    }
}


void Estimate::computeDissimilarity(Dissimilarity &out) {
    for (int a = 0; a < NUM_PIECES; ++a) {
        for (int b = 0; b < NUM_PIECES; ++b) {
            for (int d = 0; d < 4; ++d)
                out[a][b][d] = INFINITY;
        }
    }


    for (auto [ax, ay, a] : Array2DIterator(PER_SIDE, PER_SIDE)) {
        PieceImage &A = pieces[ax][ay];
        for(auto [bx, by, b] : Array2DIterator(PER_SIDE, PER_SIDE)) {
            PieceImage &B = pieces[bx][by];
            for (int d = 0; d < 4; ++d) {
                float su = 0;
                for (int k = 0; k < PIECE_SIZE; ++k) {
                    int ax = 0, ay = k;
                    int bx = 1, by = k;
                    int cx = PIECE_SIZE - 1, cy = k;

                    if (d & 1) { // opposite
                        ax = PIECE_SIZE - 1 - ax;
                        bx = PIECE_SIZE - 1 - bx;
                        cx = PIECE_SIZE - 1 - cx;
                    }

                    if (d & 2) { // perpendicular
                        std::swap(ax, ay);
                        std::swap(bx, by);
                        std::swap(cx, cy);
                    }


                    for (int ch = 0; ch < 3; ++ch) {
                        float p = A[ax][ay][ch], q = A[bx][by][ch],
                              r = B[cx][cy][ch];
                        su += std::abs(2 * p - q - r);
                    }
                }
                out[a][b][d] = su;
            }
        }
    }
}

void Estimate::initBest(void) {
    for (int a = 0; a < NUM_PIECES; ++a) {
        for (int b = 0; b < NUM_PIECES; ++b) {
            if (a == b) {
                continue;
            }

            for (int d = 0; d < 4; ++d) {
                // printf("Insert %d %d %d\n",a,d,b);
                best[a][d].insert(b, dis[a][b][d]);
            }
        }
    }
}

bool Estimate::isBestBuddies(int a, int b, int d) {
    /*
     * This is needed to tie-break multiple "best buddies" with compatibility 0,
     * e.g. when the current dissimilarity equals the second-best disimilarity
     */
    return C[a][b][d] >= std::max(bestC[a][d], 1e-6f) &&
           C[b][a][opp(d)] >= std::max(bestC[b][opp(d)], 1e-6f);
}

float Estimate::mutualScore(int a, int b, int d) {
    return (C[a][b][d] + C[b][a][opp(d)]) / 2.;
}

void Estimate::updateC(void) {
    for (int a = 0; a < NUM_PIECES; ++a) {
        for (int d = 0; d < 4; ++d) {
            bestC[a][d] = -1e10;
        }

        for (int b = 0; b < NUM_PIECES; ++b) {
            if (a == b) {
                continue;
            }

            for (int d = 0; d < 4; ++d) {
                C[a][b][d] = -1e-10;
            }
        }
    }

    for (int a = 0; a < NUM_PIECES; ++a) {
        for (int b = 0; b < NUM_PIECES; ++b) {
            if (a == b) {
                continue;
            }

            for (int d = 0; d < 4; ++d) {
                float u = dis[a][b][d];
                float v = best[a][d].secondBest();        // second-best
                float newScore = C[a][b][d] = 1. - u / v; // update C!
                bestC[a][d] = std::max(bestC[a][d], newScore);
            }
        }
    }
}

int Estimate::computeBestBuddies(void) {
    updateC();

    // Adjacency list of best buddies
    std::vector<std::pair<int, int>> buddyAdj[NUM_PIECES];

    for (int a = 0; a < NUM_PIECES; ++a) {
        for (int b = 0; b < NUM_PIECES; ++b) {
            if (a == b) {
                continue;
            }

            for (int d = 0; d < 4; ++d) {
                if (isBestBuddies(a, b, d)) {
                    buddyAdj[a].push_back({b, d});
                }
            }
        }
    }

    BestOne<int, double> bestCandidate[5];
    // for (int i = 0; i <= 4; ++i) {
    //     bestCandidate[i] = -1;
    //     candidateScore[i] = INFINITY;
    // }

    // Find suitable candidate pieces for seed tile
    for (int a = 0; a < NUM_PIECES; ++a) {
        auto &xs = buddyAdj[a];
        assert(xs.size() <= 4);

        int as[4] = {-1, -1, -1, -1};
        for (auto [b, d] : xs) {
            assert(as[d] < 0);
            assert(b >= 0);
            as[d] = b;
        }

        int c = xs.size();

        double s = 0;
        for (int i = 0; i < 4; ++i) {
            s += mutualScore(a, as[i], i);
        }

        auto &cur = bestCandidate[c];
        cur.update(s, a);
    }

    if (!bestCandidate[4].empty()) {
        return bestCandidate[4].get();
    }

    /*
     * We are unable to find a position with all best-buddies.
     * So enforce a significantly relaxed condition,
     * decreasing the number of best buddies until a candidate is found.
     */
    LOG("Relaxing best buddies condition!\n");
    for (int k = 3; k >= 1; --k) {
        if (!bestCandidate[k].empty()) {
            return bestCandidate[k].get();
        }
    }

    // If there are no candidates, choose a random piece
    return rand() % NUM_PIECES;
}

std::vector<int> Estimate::greedyPlacement(int seed) {
    typedef std::pair<float, std::pair<int, Pos>> QueryPair;
    std::set<QueryPair, std::greater<>> Q;
    // Populate Q with seed's best buddies

    for (int a = seed, b = 0; b < NUM_PIECES; ++b) {
        if (a == b) {
            continue;
        }

        for (int d = 0; d < 4; ++d) {
            if (isBestBuddies(a, b, d)) {
                Q.insert({mutualScore(a, b, d),
                          {b, {moore[d][0], moore[d][1]}}});
            }
        }
    }


    bool pieceIsFree[NUM_PIECES];
    std::fill(pieceIsFree, pieceIsFree + NUM_PIECES, true);
    std::set<int> remainingPieces;

    // TODO: Maintain only the boundary of remaining pieces instead.
    for (int i = 0; i < NUM_PIECES; ++i) {
        remainingPieces.insert(i);
    }

    // Place seed
    Grid grid;
    grid.place({0, 0}, seed);
    pieceIsFree[seed] = false;
    int placed = 1, rem = NUM_PIECES - 1;
    remainingPieces.erase(seed);

    while (rem) {
        BestOne<Placement, double, std::greater<double>> bestCandidate;
        // Pos cur_pos;
        // int selectedPiece = -1;
        // double a_score = -1;

        while (Q.size()) {
            // Place bestCandidate found piece
            auto t = *Q.begin();
            Q.erase(Q.begin());
            Pos curPos = t.second.second;
            int index = t.second.first;
            if (pieceIsFree[index] && grid.canPlace(curPos)) {
                bestCandidate.update(t.first, {index, curPos});
                break;
            }
        }

        if (bestCandidate.empty()) {
            LOG("No more pieces, updating C\n");

            updateC();

            /*
             * Iterate through all neighbours in grid, and use the bestCandidate neighbour.
             */

            // int bestCandidate = -1;
            // double best_score = -INFINITY;
            // Pos best_pos;
            for (auto [pos, p] : grid.places) {
                // Check if there are any unfilled neighbours
                for (int d = 0; d < 4; ++d) {
                    int dx = moore[d][0], dy = moore[d][1];
                    Pos new_pos{pos.x + dx, pos.y + dy};
                    if (grid.canPlace(new_pos)) {
                        // TODO: Loop only over unplaced pieces.
                        for (int k = 0; k < NUM_PIECES; ++k) {
                            if (!pieceIsFree[k]) {
                                continue;
                            }

                            double sc = mutualScore(p, k, d);
                            bestCandidate.update(sc, {k, new_pos});
                        }
                    }
                }
            }

            if (!bestCandidate.empty()) {
                Placement p = bestCandidate.get();
                LOG("Found suitable neighbour %d at (%d, %d) [score %f]\n",
                        p.piece, p.pos.x, p.pos.y, bestCandidate.getScore());

            }

            // break;
            if (bestCandidate.empty()) {
                LOG("Could not find any piece\n");
                break;
            }
        }

        int selectedPiece = bestCandidate.get().piece;
        Pos bestPos = bestCandidate.get().pos;

        LOG("Placing %d at (%d, %d) [score %f]\n",
                selectedPiece, bestPos.x, bestPos.y, bestCandidate.getScore());

        assert(pieceIsFree[selectedPiece]);
        grid.place(bestPos, selectedPiece);
        pieceIsFree[selectedPiece] = false;

        ++placed;
        --rem;

        remainingPieces.erase(selectedPiece);

        for (int b = 0; b < NUM_PIECES; ++b) {
            if (b == selectedPiece) {
                continue;
            }

            for (int d = 0; d < 4; ++d) {
                // LOG("Remove %d %d %d\n",selectedPiece,d,b);
                best[selectedPiece][d].remove(b);

                // LOG("Remove %d %d %d\n",b,opp(d),selectedPiece);
                best[b][opp(d)].remove(selectedPiece);
            }
        }

        /*
         * Add best buddies of placed piece to pool.
         * If we already have C and bestC updated, then we can immediately loop.
         */
        for (int b = 0; b < NUM_PIECES; ++b) {
            if (selectedPiece == b) {
                continue;
            }

            if (!pieceIsFree[b]) {
                continue;
            }

            for (int d = 0; d < 4; ++d) {
                int dx = moore[d][0], dy = moore[d][1];
                Pos newPos = {bestPos.x + dx, bestPos.y + dy};
                if (grid.canPlace(newPos) && isBestBuddies(selectedPiece, b, d)) {
                    Q.insert({mutualScore(selectedPiece, b, d), {b, newPos}});
                }
            }
        }
    }

    LOG("%d pieces placed, %d pieces left\n", placed, rem);
    LOG("%d %d %d %d\n",grid.minX, grid.maxX, grid.minY, grid.maxY);

    for(int i=0;i<NUM_PIECES;++i) {
        assert(!pieceIsFree[i]);
    }

    assert(remainingPieces.empty());

    return grid.extractPermutation();
}
