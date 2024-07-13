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
		board[6][i] = { PAWN, WHITE }; // Init seventh row with white pawns
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

/** Function to determine if check **/
bool is_check(const ChessBoard& board, Color) {
	std::pair<int, int> kingPos; // pair to hold the King's position
	for(int row = 0; row < NUM_TILES; row++) {
		for(int col = 0; col < NUM_TILES; col++) {
			if(board.board[row][col].type == KING && board.board[row][col].color == color) {
				kingPos = { row, col };
				break;
			}
		}
	}

	for(int row = 0; row < NUM_TILES; row++) {
		for(int col = 0; col < NUM_TILES; col++) {
			if(board.board[row][col],color != color && board.board[row][col] != NONE) {
				if(is_valid_move(board, row, col, kingPos.first, kingPos.second)) {
					return true;
				}
			}
		}
	}
	return false;
}

/** Function to determine if Checkmate **/
bool is_checkmate(ChessBoard& board, Color color) {
	if(!is_check(board, color)) {
		return false;
	}

	for(int row = 0; row < NUM_TILES; row++) {
		for(int col = 0; col < NUM_TILES; col++) {
			if(board.board[row][col].color == color) {
				std::vector<std::pair<int, int>> moves = get_valid_moves(board, row, col);
				for(auto move : moves) {
					ChessBoard tempBoard = board;
					tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
					tempBoard.board[row][col] = { EMPTY, NONE };
					if(!is_check(tempBoard, color)) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

/** Function to detect Stalemate **/
bool is_stalemate(ChessBoard& board, Color color) {
	if(is_check(board, color)) {
		return false;
	}

	for(int row = 0; row < NUM_TILES; row++) {
		for(int col = 0; col < NUM_TILES; col++) {
			if(board.board[row][col].color == color) {
				std::vector<std::pair<int, int>> moves = get_valid_moves(board, row, col)
				if(!moves.empty()) {
					return false;
				}
			}
		}
	}
	return true;
}
