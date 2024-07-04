#include "board.h"
const int NUM_TILES = 8;

/** Constructor **/
ChessBoard::ChessBoard() {
	// Initialize Board with Empty Pieces
	board = std::vector<std::vector<Piece>>(8, std::vector<Piece>(8, { EMPTY, NONE }));
	setup_pieces(); // Set up the initial positions
}

/** Piece Setup Method **/
void ChessBoard::setup_pieces() {
	/* Pawn Setup */
	for(int i = 0; i < NUM_TILES; i++) {
		board[1][i] = { PAWN, BLACK }; // Init second row with black pawns
		board[6][i] = { PAWN, BLACK }; // Init seventh row with white pawns
	}

	/* Black Piece Setup */
	board[0][0] = board[0][7] = { ROOK, BLACK };
	board[0][1] = board[0][6] = { KNIGHT, BLACK };
	board[0][2] = board[0][5] = { BISHOP, BLACK };
	board[0][3] = { QUEEN, BLACK };
	board[0][4] = { KING, BLACK };

	/* White Piece Setup */
	board[7][0] = board[7][7] = { ROOK, WHITE };
	board[7][1] = board[7][6] = { KNIGHT, WHITE };
	board[7][2] = board[7][5] = { BISHOP, WHITE };
	board[7][3] = { QUEEN, WHITE };
	board[7][4] = { KING, WHITE };
}
