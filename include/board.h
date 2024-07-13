/** Header File declaring class ChessBoard **/
#ifndef BOARD_H // If not defined
#define BOARD_H // Define

#include <vector>
#include "piece.h"

class ChessBoard {
public:
	ChessBoard();
	void setup_pieces();
	std::vector<std::vector<Piece>> board;
};

bool is_valid_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);
bool is_check(ChessBoard& board, Color color);
bool is_checkmate(ChessBoard& board, Color color);
bool is_stalemate(ChessBoard& board, Color color);

#endif // BOARD_H
