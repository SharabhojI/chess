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

#endif // BOARD_H
