#include "game.h"

#include <iostream>

int main() {
	printInstructions();

	bool playAgain = true;
	while (playAgain) {
		GameBoard board = makeBoard();
		GameStatus status = GameStatus::Ongoing;

		while (status == GameStatus::Ongoing) {
			printBoard(board);
			std::cout << "Player " << (board.currentPlayer == Player::Player1 ? 1 : 2)
					  << "'s turn.\n";

			Move move;
			const MoveInputResult inputResult = readMove(board, move);
			if (inputResult == MoveInputResult::InputEnded) {
				std::cout << "Input ended. Exiting cleanly.\n";
				return 0;
			}
			if (inputResult == MoveInputResult::EndGameRequested) {
				endGameEarly(status);
				continue;
			}

			const MoveResult result = play(board, move);
			if (result == MoveResult::Hit) {
				std::cout << "Hit!\n";
			} else if (result == MoveResult::Miss) {
				std::cout << "Miss.\n";
			} else {
				std::cout << "That move is invalid. Please choose another coordinate.\n";
				continue;
			}
			status = gameStatus(board);
		}

		if (status == GameStatus::Player1Wins) {
			std::cout << "Player 1 wins!\n";
		} else if (status == GameStatus::Player2Wins) {
			std::cout << "Player 2 wins!\n";
		} else if (status == GameStatus::EndedEarly) {
			std::cout << "Game ended early.\n";
		}
		playAgain = askPlayAgain();
	}

	std::cout << "Thanks for playing Battleship.\n";
	return 0;
}
