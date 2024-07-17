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

bool is_valid_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    std::cout << "Checking move from (" << srcRow << "," << srcCol << ") to (" << destRow << "," << destCol << ")" << std::endl;

    // Check if the move is within bounds
    if (srcRow < 0 || srcRow >= NUM_TILES || srcCol < 0 || srcCol >= NUM_TILES ||
        destRow < 0 || destRow >= NUM_TILES || destCol < 0 || destCol >= NUM_TILES) {
        std::cout << "Move is out of bounds" << std::endl;
        return false;
    }

    Piece srcPiece = board.board[srcRow][srcCol];
    Piece destPiece = board.board[destRow][destCol];

    std::cout << "Source piece: " << srcPiece.type << ", " << srcPiece.color << std::endl;
    std::cout << "Destination piece: " << destPiece.type << ", " << destPiece.color << std::endl;

    // Check if the destination is occupied by a piece of the same color
    if (destPiece.color == srcPiece.color && destPiece.type != EMPTY) {
        std::cout << "Cannot capture own piece" << std::endl;
        return false;
    }

    // Check for castling
    if (srcPiece.type == KING && std::abs(destCol - srcCol) == 2) {
        // Castling attempt
        if (srcPiece.color == WHITE && srcRow == 7 && srcCol == 4) {
            if (destCol == 6 && !board.white_king_moved && !board.white_kingside_rook_moved) {
                // White kingside castling
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, WHITE);
            } else if (destCol == 2 && !board.white_king_moved && !board.white_queenside_rook_moved) {
                // White queenside castling
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, WHITE);
            }
        } else if (srcPiece.color == BLACK && srcRow == 0 && srcCol == 4) {
            if (destCol == 6 && !board.black_king_moved && !board.black_kingside_rook_moved) {
                // Black kingside castling
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, BLACK);
            } else if (destCol == 2 && !board.black_king_moved && !board.black_queenside_rook_moved) {
                // Black queenside castling
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, BLACK);
            }
        }
    }

    switch (srcPiece.type) {
        case PAWN:
            if (srcPiece.color == WHITE) {
                // Standard one square forward move
                if (destRow == srcRow - 1 && destCol == srcCol && destPiece.type == EMPTY) {
                    std::cout << "Valid white pawn forward move" << std::endl;
                    return true;
                }
                // Initial two squares forward move
                if (srcRow == 6 && destRow == srcRow - 2 && destCol == srcCol &&
                    destPiece.type == EMPTY && board.board[srcRow - 1][srcCol].type == EMPTY) {
                    std::cout << "Valid white pawn initial two-square move" << std::endl;
                    return true;
                }
                // Diagonal capture
                if (destRow == srcRow - 1 && std::abs(destCol - srcCol) == 1 && destPiece.color == BLACK) {
                    std::cout << "Valid white pawn diagonal capture" << std::endl;
                    return true;
                }
            } else if (srcPiece.color == BLACK) {
                // Standard one square forward move
                if (destRow == srcRow + 1 && destCol == srcCol && destPiece.type == EMPTY) {
                    std::cout << "Valid black pawn forward move" << std::endl;
                    return true;
                }
                // Initial two squares forward move
                if (srcRow == 1 && destRow == srcRow + 2 && destCol == srcCol &&
                    destPiece.type == EMPTY && board.board[srcRow + 1][srcCol].type == EMPTY) {
                    std::cout << "Valid black pawn initial two-square move" << std::endl;
                    return true;
                }
                // Diagonal capture
                if (destRow == srcRow + 1 && std::abs(destCol - srcCol) == 1 && destPiece.color == WHITE) {
                    std::cout << "Valid black pawn diagonal capture" << std::endl;
                    return true;
                }
            }
            std::cout << "Invalid pawn move" << std::endl;
            break;

        case KNIGHT:
            if ((abs(destRow - srcRow) == 2 && abs(destCol - srcCol) == 1) ||
                (abs(destRow - srcRow) == 1 && abs(destCol - srcCol) == 2)) {
                return true;
            }
            break;

        case BISHOP:
            if (abs(destRow - srcRow) == abs(destCol - srcCol)) {
                int rowDirection = (destRow - srcRow) / abs(destRow - srcRow);
                int colDirection = (destCol - srcCol) / abs(destCol - srcCol);
                for (int i = 1; i < abs(destRow - srcRow); i++) {
                    if (board.board[srcRow + i * rowDirection][srcCol + i * colDirection].type != EMPTY) {
                        return false;
                    }
                }
                return true;
            }
            break;

        case ROOK:
            if (srcRow == destRow || srcCol == destCol) {
                if (srcRow == destRow) {
                    int direction = (destCol - srcCol) / abs(destCol - srcCol);
                    for (int i = 1; i < abs(destCol - srcCol); i++) {
                        if (board.board[srcRow][srcCol + i * direction].type != EMPTY) {
                            return false;
                        }
                    }
                } else {
                    int direction = (destRow - srcRow) / abs(destRow - srcRow);
                    for (int i = 1; i < abs(destRow - srcRow); i++) {
                        if (board.board[srcRow + i * direction][srcCol].type != EMPTY) {
                            return false;
                        }
                    }
                }
                return true;
            }
            break;

        case QUEEN:
            if ((srcRow == destRow || srcCol == destCol) ||
                (abs(destRow - srcRow) == abs(destCol - srcCol))) {
                if (srcRow == destRow || srcCol == destCol) {
                    if (srcRow == destRow) {
                        int direction = (destCol - srcCol) / abs(destCol - srcCol);
                        for (int i = 1; i < abs(destCol - srcCol); i++) {
                            if (board.board[srcRow][srcCol + i * direction].type != EMPTY) {
                                return false;
                            }
                        }
                    } else {
                        int direction = (destRow - srcRow) / abs(destRow - srcRow);
                        for (int i = 1; i < abs(destRow - srcRow); i++) {
                            if (board.board[srcRow + i * direction][srcCol].type != EMPTY) {
                                return false;
                            }
                        }
                    }
                } else {
                    int rowDirection = (destRow - srcRow) / abs(destRow - srcRow);
                    int colDirection = (destCol - srcCol) / abs(destCol - srcCol);
                    for (int i = 1; i < abs(destRow - srcRow); i++) {
                        if (board.board[srcRow + i * rowDirection][srcCol + i * colDirection].type != EMPTY) {
                            return false;
                        }
                    }
                }
                return true;
            }
            break;

        case KING:
            if (abs(destRow - srcRow) <= 1 && abs(destCol - srcCol) <= 1) {
                return true;
            }
            break;

        default:
            return false;
    }

    return false;
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
                if (is_valid_move(board, r, c, row, col)) {
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
            if (is_valid_move(board, row, col, destRow, destCol)) {
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
				std::vector<std::pair<int, int>> moves = get_valid_moves(board, row, col);
				if(!moves.empty()) {
					return false;
				}
			}
		}
	}
	return true;
}
