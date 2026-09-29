#ifndef BATTLESHIP_GAME_H
#define BATTLESHIP_GAME_H

#include <array>

constexpr int BoardSize = 10;

enum class Player {
    Player1,
    Player2
};

enum class ShotResult {
    Unknown,
    Hit,
    Miss
};

enum class GameStatus {
    Ongoing,
    Player1Wins,
    Player2Wins,
    EndedEarly
};

enum class MoveResult {
    Invalid,
    Hit,
    Miss
};

enum class MoveInputResult {
    MoveReady,
    EndGameRequested,
    InputEnded
};

struct Move {
    int row;
    int column;
};

struct GameBoard {
    std::array<std::array<bool, BoardSize>, BoardSize> ships[2];
    std::array<std::array<ShotResult, BoardSize>, BoardSize> shots[2];
    int shipsRemaining[2];
    Player currentPlayer;

    GameBoard();
};

GameBoard makeBoard();
void printBoard(const GameBoard& board);
MoveResult play(GameBoard& board, Move move);
GameStatus gameStatus(const GameBoard& board);
MoveInputResult readMove(const GameBoard& board, Move& move);
void endGameEarly(GameStatus& status);
void printInstructions();
bool askPlayAgain();

#endif
