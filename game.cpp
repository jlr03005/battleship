// AI Disclaimer: This code was written with minimal AI assistance.
// Used AI for: syntax checking and debugging only.
// Core logic and problem-solving approach are my own work.
// Game Project 01: Battleship
#include "game.h"

#include <cctype>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace {

int playerIndex(Player player) {
	return player == Player::Player1 ? 0 : 1;
}

Player otherPlayer(Player player) {
	return player == Player::Player1 ? Player::Player2 : Player::Player1;
}

bool isInsideBoard(Move move) {
	return move.row >= 0 && move.row < BoardSize &&
		   move.column >= 0 && move.column < BoardSize;
}

bool isValidCoordinate(const std::string& coordinate, Move& move) {
	if (coordinate.size() < 2 || coordinate.size() > 3) {
		return false;
	}

	const char rowLabel = static_cast<char>(
		std::toupper(static_cast<unsigned char>(coordinate[0])));
	if (rowLabel < 'A' || rowLabel >= 'A' + BoardSize) {
		return false;
	}
	if (coordinate.size() == 3 &&
		(coordinate[1] != '1' || coordinate[2] != '0')) {
		return false;
	}

	int columnNumber = 0;
	for (std::size_t index = 1; index < coordinate.size(); ++index) {
		const unsigned char character =
			static_cast<unsigned char>(coordinate[index]);
		if (!std::isdigit(character)) {
			return false;
		}
		columnNumber = columnNumber * 10 + (coordinate[index] - '0');
	}

	if (columnNumber < 1 || columnNumber > BoardSize) {
		return false;
	}

	move = {rowLabel - 'A', columnNumber - 1};
	return true;
}

bool isEndGameCommand(std::string command) {
	for (char& character : command) {
		character = static_cast<char>(
			std::toupper(static_cast<unsigned char>(character)));
	}
	return command == "QUIT";
}

}

GameBoard::GameBoard() : shipsRemaining{17, 17}, currentPlayer(Player::Player1) {
	for (int player = 0; player < 2; ++player) {
		for (int row = 0; row < BoardSize; ++row) {
			ships[player][row].fill(false);
			shots[player][row].fill(ShotResult::Unknown);
		}
	}
}

GameBoard makeBoard() {
	GameBoard board;
	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());
	const std::array<int, 5> shipLengths{{5, 4, 3, 3, 2}};
	std::uniform_int_distribution<int> coordinate(0, BoardSize - 1);
	std::uniform_int_distribution<int> direction(0, 1);

	for (int player = 0; player < 2; ++player) {
		for (int length : shipLengths) {
			bool placed = false;
			while (!placed) {
				const int startRow = coordinate(generator);
				const int startColumn = coordinate(generator);
				const bool horizontal = direction(generator) == 0;
				bool canPlace = true;

				for (int offset = 0; offset < length; ++offset) {
					const int row = startRow + (horizontal ? 0 : offset);
					const int column = startColumn + (horizontal ? offset : 0);
					if (row >= BoardSize || column >= BoardSize ||
						board.ships[player][row][column]) {
						canPlace = false;
						break;
					}
				}

				if (canPlace) {
					for (int offset = 0; offset < length; ++offset) {
						const int row = startRow + (horizontal ? 0 : offset);
						const int column = startColumn + (horizontal ? offset : 0);
						board.ships[player][row][column] = true;
					}
					placed = true;
				}
			}
		}
	}

	return board;
}

void printBoard(const GameBoard& board) {
	const int shooter = playerIndex(board.currentPlayer);
	std::cout << "\nShots fired by Player " << shooter + 1 << ":\n    ";
	for (int column = 1; column <= BoardSize; ++column) {
		std::cout << column << (column == 10 ? " " : "  ");
	}
	std::cout << '\n';

	for (int row = 0; row < BoardSize; ++row) {
		std::cout << static_cast<char>('A' + row) << "   ";
		for (int column = 0; column < BoardSize; ++column) {
			char symbol = '~';
			if (board.shots[shooter][row][column] == ShotResult::Hit) {
				symbol = 'X';
			} else if (board.shots[shooter][row][column] == ShotResult::Miss) {
				symbol = 'o';
			}
			std::cout << symbol << "  ";
		}
		std::cout << '\n';
	}
	std::cout << "~ = unknown, X = hit, o = miss\n";
}

MoveResult play(GameBoard& board, Move move) {
	if (!isInsideBoard(move)) {
		return MoveResult::Invalid;
	}

	const int shooter = playerIndex(board.currentPlayer);
	const int target = playerIndex(otherPlayer(board.currentPlayer));
	if (board.shots[shooter][move.row][move.column] != ShotResult::Unknown) {
		return MoveResult::Invalid;
	}

	const bool hit = board.ships[target][move.row][move.column];
	board.shots[shooter][move.row][move.column] =
		hit ? ShotResult::Hit : ShotResult::Miss;
	if (hit) {
		--board.shipsRemaining[target];
	}
	board.currentPlayer = otherPlayer(board.currentPlayer);

	return hit ? MoveResult::Hit : MoveResult::Miss;
}

GameStatus gameStatus(const GameBoard& board) {
	if (board.shipsRemaining[1] == 0) {
		return GameStatus::Player1Wins;
	}
	if (board.shipsRemaining[0] == 0) {
		return GameStatus::Player2Wins;
	}
	return GameStatus::Ongoing;
}

MoveInputResult readMove(const GameBoard& board, Move& move) {
	std::string line;
	while (true) {
		std::cout << "Enter a coordinate (A1-J10), or QUIT to end the game: ";
		if (!std::getline(std::cin, line)) {
			return MoveInputResult::InputEnded;
		}

		std::istringstream input(line);
		std::string coordinate;
		std::string extraInput;
		if (!(input >> coordinate) || (input >> extraInput)) {
			std::cout << "Invalid coordinate. Use a single coordinate from A1 to J10.\n";
			continue;
		}
		if (isEndGameCommand(coordinate)) {
			return MoveInputResult::EndGameRequested;
		}
		if (!isValidCoordinate(coordinate, move)) {
			std::cout << "Invalid coordinate. Use a single coordinate from A1 to J10.\n";
			continue;
		}

		const int shooter = playerIndex(board.currentPlayer);
		if (board.shots[shooter][move.row][move.column] != ShotResult::Unknown) {
			std::cout << "You already fired at that coordinate. Choose another.\n";
			continue;
		}
		return MoveInputResult::MoveReady;
	}
}

void endGameEarly(GameStatus& status) {
	status = GameStatus::EndedEarly;
}

void printInstructions() {
	std::cout << "Battleship\n"
			  << "Two players take turns sharing the device. Each fleet is placed "
				 "randomly on a hidden 10x10 grid.\n"
			  << "Call out a coordinate from A1 to J10 on your turn. The board "
				 "shows your previous shots: X is a hit and o is a miss.\n"
			  << "The first player to hit all 17 ship spaces in the opponent's "
				 "fleet wins. Enter coordinates such as B7 or J10. Type QUIT to "
				 "end the current game early.\n\n";
}

bool askPlayAgain() {
	std::string line;
	while (true) {
		std::cout << "Play again? (Y/N): ";
		if (!std::getline(std::cin, line)) {
			return false;
		}

		std::istringstream input(line);
		std::string answer;
		std::string extraInput;
		if ((input >> answer) && !(input >> extraInput) && answer.size() == 1) {
			const char choice = static_cast<char>(
				std::toupper(static_cast<unsigned char>(answer[0])));
			if (choice == 'Y') {
				return true;
			}
			if (choice == 'N') {
				return false;
			}
		}
		std::cout << "Please enter Y to play again or N to exit.\n";
	}
}
