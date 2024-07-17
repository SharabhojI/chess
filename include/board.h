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

	// For tracking castling eligibility
    bool white_king_moved;
    bool black_king_moved;
    bool white_kingside_rook_moved;
    bool white_queenside_rook_moved;
    bool black_kingside_rook_moved;
    bool black_queenside_rook_moved;
};

bool is_valid_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);
std::vector<std::pair<int, int>> get_valid_moves(const ChessBoard& board, int row, int col);
bool is_check(const ChessBoard& board, Color color);
bool is_checkmate(ChessBoard& board, Color color);
bool is_stalemate(ChessBoard& board, Color color);

// Function declarations for castling logic
bool is_castling_valid(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol, Color color);
bool is_square_attacked(const ChessBoard& board, int row, int col, Color attackingColor);

#endif // BOARD_H
