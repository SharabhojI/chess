#include "board.h"
#include <cmath>
#include <iostream>

const int NUM_TILES = 8;

/** Constructor **/
ChessBoard::ChessBoard() {
	// Initialize Board with Empty Pieces
	board = std::vector<std::vector<Piece>>(8, std::vector<Piece>(8, { EMPTY, NONE }));
	setup_pieces(); // Set up the initial positions

    // Initialize castling flags
    white_king_moved = false;
    black_king_moved = false;
    white_kingside_rook_moved = false;
    white_queenside_rook_moved = false;
    black_kingside_rook_moved = false;
    black_queenside_rook_moved = false;
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

bool is_valid_pawn_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    int direction = (board.board[srcRow][srcCol].color == WHITE) ? -1 : 1;
    int startRow = (board.board[srcRow][srcCol].color == WHITE) ? 6 : 1;

    // Forward move
    if (srcCol == destCol) {
        if (destRow == srcRow + direction && board.board[destRow][destCol].type == EMPTY) {
            return true;
        }
        if (srcRow == startRow && destRow == srcRow + 2 * direction &&
            board.board[srcRow + direction][srcCol].type == EMPTY &&
            board.board[destRow][destCol].type == EMPTY) {
            return true;
        }
    }

    // Capture move
    if (destRow == srcRow + direction && std::abs(destCol - srcCol) == 1) {
        if (board.board[destRow][destCol].type != EMPTY &&
            board.board[destRow][destCol].color != board.board[srcRow][srcCol].color) {
            return true;
        }
        // En passant
        if (board.board[destRow][destCol].type == EMPTY &&
            board.last_move == std::make_pair(destRow - direction, destCol) &&
            board.last_move_start == std::make_pair(destRow + direction, destCol) &&
            board.last_move_was_pawn_double &&
            board.board[destRow - direction][destCol].type == PAWN &&
            board.board[destRow - direction][destCol].color != board.board[srcRow][srcCol].color) {
            return true;
        }
    }

    return false;
}

bool is_valid_knight_move(int srcRow, int srcCol, int destRow, int destCol) {
    int rowDiff = std::abs(destRow - srcRow);
    int colDiff = std::abs(destCol - srcCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool is_valid_bishop_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    if (std::abs(destRow - srcRow) != std::abs(destCol - srcCol)) {
        return false;
    }
    int rowDir = (destRow > srcRow) ? 1 : -1;
    int colDir = (destCol > srcCol) ? 1 : -1;
    for (int i = 1; i < std::abs(destRow - srcRow); ++i) {
        if (board.board[srcRow + i * rowDir][srcCol + i * colDir].type != EMPTY) {
            return false;
        }
    }
    return true;
}

bool is_valid_rook_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    if (srcRow != destRow && srcCol != destCol) {
        return false;
    }
    int rowDir = (destRow > srcRow) ? 1 : (destRow < srcRow) ? -1 : 0;
    int colDir = (destCol > srcCol) ? 1 : (destCol < srcCol) ? -1 : 0;
    for (int i = 1; i < std::max(std::abs(destRow - srcRow), std::abs(destCol - srcCol)); ++i) {
        if (board.board[srcRow + i * rowDir][srcCol + i * colDir].type != EMPTY) {
            return false;
        }
    }
    return true;
}

bool is_valid_queen_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    return is_valid_bishop_move(board, srcRow, srcCol, destRow, destCol) ||
           is_valid_rook_move(board, srcRow, srcCol, destRow, destCol);
}

bool is_valid_king_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    int rowDiff = std::abs(destRow - srcRow);
    int colDiff = std::abs(destCol - srcCol);

    // Normal king move
    if (rowDiff <= 1 && colDiff <= 1) {
        return true;
    }

    // Castling
    if (rowDiff == 0 && colDiff == 2) {
        Color color = board.board[srcRow][srcCol].color;
        if (color == WHITE && !board.white_king_moved) {
            if (destCol > srcCol && !board.white_kingside_rook_moved) {
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, color);
            } else if (destCol < srcCol && !board.white_queenside_rook_moved) {
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, color);
            }
        } else if (color == BLACK && !board.black_king_moved) {
            if (destCol > srcCol && !board.black_kingside_rook_moved) {
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, color);
            } else if (destCol < srcCol && !board.black_queenside_rook_moved) {
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, color);
            }
        }
    }

    return false;
}

bool is_valid_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol, Color currentTurn) {
    // Check if the move is within bounds
    if (srcRow < 0 || srcRow >= NUM_TILES || srcCol < 0 || srcCol >= NUM_TILES ||
        destRow < 0 || destRow >= NUM_TILES || destCol < 0 || destCol >= NUM_TILES) {
        return false;
    }

    Piece srcPiece = board.board[srcRow][srcCol];
    Piece destPiece = board.board[destRow][destCol];

    // Check if the destination is occupied by a piece of the same color
    if (destPiece.color == srcPiece.color && destPiece.type != EMPTY) {
        return false;
    }

    // Check if the source piece exists and belongs to the current player
    if (srcPiece.type == EMPTY || srcPiece.color != currentTurn) {
        return false;
    }

    bool valid_move = false;

    switch (srcPiece.type) {
        case PAWN:
            valid_move = is_valid_pawn_move(board, srcRow, srcCol, destRow, destCol);
            break;
        case KNIGHT:
            valid_move = is_valid_knight_move(srcRow, srcCol, destRow, destCol);
            break;
        case BISHOP:
            valid_move = is_valid_bishop_move(board, srcRow, srcCol, destRow, destCol);
            break;
        case ROOK:
            valid_move = is_valid_rook_move(board, srcRow, srcCol, destRow, destCol);
            break;
        case QUEEN:
            valid_move = is_valid_queen_move(board, srcRow, srcCol, destRow, destCol);
            break;
        case KING:
            valid_move = is_valid_king_move(board, srcRow, srcCol, destRow, destCol);
            break;
        default:
            return false;
    }

    // Check if the move would result in self-check
    if (valid_move && would_be_in_check(board, srcRow, srcCol, destRow, destCol)) {
        return false;
    }

    return valid_move;
}

bool is_castling_valid(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol, Color color) {
    // Check if the king is in check
    if (is_check(board, color)) {
        return false;
    }

    // Check if the squares between the king and rook are empty
    int step = (destCol > srcCol) ? 1 : -1;
    for (int col = srcCol + step; col != destCol; col += step) {
        if (board.board[srcRow][col].type != EMPTY) {
            return false;
        }
    }

    // Check if the king passes through or ends up in a square that's under attack
    for (int col = srcCol; col != destCol + step; col += step) {
        if (is_square_attacked(board, srcRow, col, (color == WHITE) ? BLACK : WHITE)) {
            return false;
        }
    }

    return true;
}

bool is_square_attacked(const ChessBoard& board, int row, int col, Color attackingColor) {
    for (int r = 0; r < NUM_TILES; r++) {
        for (int c = 0; c < NUM_TILES; c++) {
            if (board.board[r][c].color == attackingColor) {
                if (is_valid_move(board, r, c, row, col, attackingColor)) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::vector<std::pair<int, int>> get_valid_moves(const ChessBoard& board, int row, int col) {
    std::vector<std::pair<int, int>> valid_moves;
    for (int destRow = 0; destRow < NUM_TILES; destRow++) {
        for (int destCol = 0; destCol < NUM_TILES; destCol++) {
            if (is_valid_move(board, row, col, destRow, destCol, board[row][col].color)) {
                valid_moves.push_back({destRow, destCol});
            }
        }
    }
    return valid_moves;
}

/** Function to determine if check **/
bool is_check(const ChessBoard& board, Color color) {
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
			if(board.board[row][col].color != color && board.board[row][col].type != EMPTY) {
				if(is_valid_move(board, row, col, kingPos.first, kingPos.second, color)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool would_be_in_check(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    ChessBoard tempBoard = board;
    tempBoard.board[destRow][destCol] = tempBoard.board[srcRow][srcCol];
    tempBoard.board[srcRow][srcCol] = { EMPTY, NONE };
    return is_check(tempBoard, tempBoard.board[destRow][destCol].color);
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
				std::vector<std::pair<int, int>> moves = get_valid_moves(board, row, col);
				if(!moves.empty()) {
					return false;
				}
			}
		}
	}
	return true;
}
