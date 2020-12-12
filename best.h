#pragma once

#include <map>
#include <set>
#include <optional>

#include <cmath>

#include "settings.h"

/*
 * Represents the best candidate of type T with a given cost.
 */
template <typename T, typename Score = int, typename Compare = std::less<Score>>
class BestOne {
    std::optional<std::pair<Score, T>> best;
public:
    BestOne() : best(std::nullopt) {}

    void update(Score score, T elem) {
        if (!best.has_value()) {
            best = {score, elem};
        } else {
            Score prevCost = best->first;
            if (Compare()(score, prevCost)) {
                best = {score, elem};
            }
        }
    }

    T get() {
        return best->second;
    }

    Score getScore() {
        return best->first;
    }

    bool empty() {
        return !best.has_value();
    }
};

/*
 * Represents a list of candidates of type T, each with a unique given cost.
 */
template <typename T, typename Score, typename Compare = std::less<Score>>
class Best {
    struct PairComparator {
        bool operator() (const std::pair<Score, T> & lhs, const std::pair<Score, T> & rhs) const
        {
            if (Compare()(lhs.first, rhs.first)) {
                return true;
            }

            if (Compare()(rhs.first, lhs.first)) {
                return false;
            }

            return lhs.second < rhs.second;
        }
    };

    std::set<std::pair<Score, T>, PairComparator> S;
    std::map<T, Score, Compare> M;
    std::optional<Score> secondBestCache;

public:
    Best() : secondBestCache(std::nullopt) {}

    void insert(T k, Score v) {
        assert(!M.count(k));
        assert(!S.count({v, k}));

        S.insert({v, k});
        M[k] = v;
        secondBestCache = std::nullopt;
    }

    void remove(T k) {
        // assert(M.count(k));

        // Note that popping from `Best` can also remove elements.
        if (!M.count(k)) {
            return;
        }
        Score v = M[k];

        // assert(S.count({v,k}));
        if (!S.count({v, k})) {
            return;
        }

        S.erase({v, k});
        secondBestCache = std::nullopt;
    }

    Score secondBest() {
        /*
         * Iterate to second element with set<>.
         * This is sufficient because we only need fast queries for second-best candidates, and fast removal.
         */
        if (secondBestCache.has_value()) {
            return secondBestCache.value();
        } else {
            if (S.size() < 2) {
                return INFINITY;
            }

            // assert(S.size()>=2);

            int c = 0;
            for (auto [v, k] : S) {
                ++c;
                if (c == 2) {
                    secondBestCache = {v};
                    return v;
                }
            }

            throw std::exception();
        }
    }

    void pop() {
        assert(S.size() >= 1);
        auto [v, k] = *S.begin();

        S.erase(S.begin());
        M.erase(M.find(k));
        secondBestCache = std::nullopt;
    }
};
