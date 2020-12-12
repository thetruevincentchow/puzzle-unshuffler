#pragma once

#include <algorithm>
#include <map>
#include <cassert>

#include "settings.h"

struct Pos {
    int x, y;
    bool operator<(const Pos &o) const {
        return std::pair(x, y) < std::pair(o.x, o.y);
    }
};

struct Grid {
    int minX, maxX, minY, maxY;
    std::map<Pos, int> places;

    Grid() : minX(0), maxX(-1), minY(0), maxY(-1) {}

    bool check_coord(int x, int a, int b);
    bool is_empty(int x, int y);
    bool is_empty(Pos pos);
    bool canPlace(int x, int y);
    bool canPlace(Pos pos);
    void place(Pos pos, int a);

    std::vector<int> extractPermutation(void);
};


/*
 * Iterator for 2D region.
 */
template<typename T>
class array2d_iterator : public std::iterator<std::forward_iterator_tag, T>{
    T rows, columns, i, j, cell;

    array2d_iterator(T _rows, T _columns, T _i, T _j, T _cell)
        : rows(_rows), columns(_columns), i(_i), j(_j), cell(_cell) {}

public:
    static array2d_iterator<T> begin(T rows, T columns) {
        return array2d_iterator(rows, columns, 0, 0, 0);
    }

    static array2d_iterator<T> end(T rows,T columns) {
        return array2d_iterator(rows, columns, rows, 0, rows * columns);
    }

    array2d_iterator() = default;

    // ++prefix operator
    array2d_iterator& operator++() {
        ++cell;
        if (j + 1 < columns) {
            ++j;
        } else {
            i++;
            j = 0;
        }
        return *this;
    }

    // postfix++ operator
    array2d_iterator operator++(T) {
        T retval = *this;
        ++(*this);
        return retval;
    }

    bool operator==(const array2d_iterator& other) const {
        return rows == other.rows && columns == other.columns
            && i == other.i && j == other.j && cell == other.cell;
    }

    bool operator!=(const array2d_iterator &other) const {
        return !(*this == other);
    }

    const std::tuple<T, T, T> operator*() const {
        return *this;
    }

    std::tuple<T, T, T> operator*() {
        return std::make_tuple(i, j, cell);
    }

    const std::tuple<T, T, T> operator->() const {
        return *this;
    }

    std::tuple<T, T, T> operator->() {
        return *this;
    }
};

struct Array2DIterator {
    int rows, columns;

public:
    Array2DIterator(int rows, int columns) : rows(rows), columns(columns) { }

    array2d_iterator<int> begin() {
        return array2d_iterator<int>::begin(rows, columns);
    }

    array2d_iterator<int> end() {
        return array2d_iterator<int>::end(rows, columns);
    }
};
