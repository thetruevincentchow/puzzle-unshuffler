#pragma once

constexpr int IMAGE_SIZE = 512;
constexpr int PIECE_SIZE = 64;
constexpr int PER_SIDE = IMAGE_SIZE / PIECE_SIZE;
constexpr int NUM_PIECES = PER_SIDE * PER_SIDE;

using Image = float[IMAGE_SIZE][IMAGE_SIZE][3];
