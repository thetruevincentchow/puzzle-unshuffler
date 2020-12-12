#include "grid.h"

bool Grid::check_coord(int x, int a, int b) {
    return std::max(b, x) - std::min(a, x) + 1 <= PER_SIDE;
}

bool Grid::is_empty(int x, int y) {
    return !places.count({x, y});
}

bool Grid::is_empty(Pos pos) {
    return !places.count(pos);
}

bool Grid::canPlace(int x, int y) {
    return check_coord(x, minX, maxX) && check_coord(y, minY, maxY) && is_empty(x, y);
}

bool Grid::canPlace(Pos pos) {
    return canPlace(pos.x, pos.y);
}

void Grid::place(Pos pos, int a) {
    assert(canPlace(pos.x, pos.y));
    places[pos] = a;

    minX = std::min(minX, pos.x);
    maxX = std::max(maxX, pos.x);

    minY = std::min(minY, pos.y);
    maxY = std::max(maxY, pos.y);
}

std::vector<int> Grid::extractPermutation(void) {
    std::vector<int> R(NUM_PIECES);
    int c = 0;
    for (int i = 0; i < PER_SIDE; ++i) {
        for (int j = 0; j < PER_SIDE; ++j) {
            auto it = places.find({minX + i, minY + j});
            R[c++] = it == places.end() ? -1 : it->second;
        }
    }
    return R;
}
