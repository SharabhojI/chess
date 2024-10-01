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
    whiteKingMoved = false;
    blackKingMoved = false;
    whiteKingsideRookMoved = false;
    whiteQueensideRookMoved = false;
    blackKingsideRookMoved = false;
    blackQueensideRookMoved = false;
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

/** Function to convert a piece to String **/
std::string piece_to_string(PieceType type) {
    switch (type) {
        case PAWN: return "";
        case KNIGHT: return "N";
        case BISHOP: return "B";
        case ROOK: return "R";
        case QUEEN: return "Q";
        case KING: return "K";
        default: return "";
    }
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
            board.lastMove == std::make_pair(destRow - direction, destCol) &&
            board.lastMoveStart == std::make_pair(destRow + direction, destCol) &&
            board.lastMoveWasPawnDouble &&
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
        if (color == WHITE && !board.whiteKingMoved) {
            if (destCol > srcCol && !board.whiteKingsideRookMoved) {
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, color);
            } else if (destCol < srcCol && !board.whiteQueensideRookMoved) {
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, color);
            }
        } else if (color == BLACK && !board.blackKingMoved) {
            if (destCol > srcCol && !board.blackKingsideRookMoved) {
                return is_castling_valid(board, srcRow, srcCol, destRow, destCol, color);
            } else if (destCol < srcCol && !board.blackQueensideRookMoved) {
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

    if (!valid_move) {
        return false;
    }

    // Check if the move would leave the king in check
    ChessBoard tempBoard = board;
    tempBoard.board[destRow][destCol] = tempBoard.board[srcRow][srcCol];
    tempBoard.board[srcRow][srcCol] = { EMPTY, NONE };
    if (is_check(tempBoard, currentTurn)) {
        return false;
    }

    return true;
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

std::vector<std::pair<int, int>> get_valid_moves(const ChessBoard& board, int row, int col, Color currentTurn) {
    std::vector<std::pair<int, int>> valid_moves;
    for (int destRow = 0; destRow < NUM_TILES; destRow++) {
        for (int destCol = 0; destCol < NUM_TILES; destCol++) {
            if (is_valid_move(board, row, col, destRow, destCol, currentTurn)) {
                valid_moves.push_back({destRow, destCol});
            }
        }
    }
    return valid_moves;
}

/** Function to determine if check **/
bool is_check(const ChessBoard& board, Color color) {
    std::pair<int, int> kingPos;
    for(int row = 0; row < NUM_TILES; row++) {
        for(int col = 0; col < NUM_TILES; col++) {
            if(board.board[row][col].type == KING && board.board[row][col].color == color) {
                kingPos = { row, col };
                goto found_king;
            }
        }
    }
    found_king:

    for(int row = 0; row < NUM_TILES; row++) {
        for(int col = 0; col < NUM_TILES; col++) {
            if(board.board[row][col].color != color && board.board[row][col].type != EMPTY) {
                if(is_valid_move(board, row, col, kingPos.first, kingPos.second, board.board[row][col].color)) {
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

    for(int srcRow = 0; srcRow < NUM_TILES; srcRow++) {
        for(int srcCol = 0; srcCol < NUM_TILES; srcCol++) {
            if(board.board[srcRow][srcCol].color == color) {
                for(int destRow = 0; destRow < NUM_TILES; destRow++) {
                    for(int destCol = 0; destCol < NUM_TILES; destCol++) {
                        if(is_valid_move(board, srcRow, srcCol, destRow, destCol, color)) {
                            ChessBoard tempBoard = board;
                            tempBoard.board[destRow][destCol] = tempBoard.board[srcRow][srcCol];
                            tempBoard.board[srcRow][srcCol] = { EMPTY, NONE };
                            if(!is_check(tempBoard, color)) {
                                return false;
                            }
                        }
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

    for(int srcRow = 0; srcRow < NUM_TILES; srcRow++) {
        for(int srcCol = 0; srcCol < NUM_TILES; srcCol++) {
            if(board.board[srcRow][srcCol].color == color) {
                for(int destRow = 0; destRow < NUM_TILES; destRow++) {
                    for(int destCol = 0; destCol < NUM_TILES; destCol++) {
                        if(is_valid_move(board, srcRow, srcCol, destRow, destCol, color)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

void ChessBoard::record_move(int srcRow, int srcCol, int destRow, int destCol) {
    Piece capturedPiece = board[destRow][destCol];
    bool wasEnPassant = (board[srcRow][srcCol].type == PAWN && srcCol != destCol && capturedPiece.type == EMPTY);
    bool wasCastling = (board[srcRow][srcCol].type == KING && std::abs(destCol - srcCol) == 2);
    bool wasPawnPromotion = (board[srcRow][srcCol].type == PAWN && (destRow == 0 || destRow == 7));

    Move move(srcRow, srcCol, destRow, destCol, capturedPiece, wasEnPassant, wasCastling, wasPawnPromotion);
    moveHistory.push_back(move);
}

bool ChessBoard::undo_last_move() {
    if (moveHistory.empty()) return false;

    Move& move = moveHistory.back();

    // Undo the move
    board[move.srcRow][move.srcCol] = board[move.destRow][move.destCol];
    board[move.destRow][move.destCol] = move.capturedPiece;

    // Handle special cases
    if (move.wasEnPassant) {
        int capturedPawnRow = (move.srcRow + move.destRow) / 2;
        board[capturedPawnRow][move.destCol] = {PAWN, (Color)(1 - board[move.srcRow][move.srcCol].color)};
    } else if (move.wasCastling) {
        int rookSrcCol = (move.destCol > move.srcCol) ? 7 : 0;
        int rookDestCol = (move.destCol > move.srcCol) ? move.destCol - 1 : move.destCol + 1;
        board[move.srcRow][rookSrcCol] = board[move.srcRow][rookDestCol];
        board[move.srcRow][rookDestCol] = {EMPTY, NONE};
    } else if (move.wasPawnPromotion) {
        board[move.srcRow][move.srcCol].type = PAWN;
    }

    // Update castling flags
    if (move.srcRow == 0 && move.srcCol == 4) blackKingMoved = false;
    if (move.srcRow == 7 && move.srcCol == 4) whiteKingMoved = false;
    if (move.srcRow == 0 && move.srcCol == 0) blackQueensideRookMoved = false;
    if (move.srcRow == 0 && move.srcCol == 7) blackKingsideRookMoved = false;
    if (move.srcRow == 7 && move.srcCol == 0) whiteQueensideRookMoved = false;
    if (move.srcRow == 7 && move.srcCol == 7) whiteKingsideRookMoved = false;

    // Update last move information
    if (moveHistory.size() > 1) {
        Move& prevMove = moveHistory[moveHistory.size() - 2];
        lastMoveStart = {prevMove.srcRow, prevMove.srcCol};
        lastMove = {prevMove.destRow, prevMove.destCol};
        lastMoveWasPawnDouble = (prevMove.srcRow == 1 && prevMove.destRow == 3) || (prevMove.srcRow == 6 && prevMove.destRow == 4);
    } else {
        lastMoveStart = {-1, -1};
        lastMove = {-1, -1};
        lastMoveWasPawnDouble = false;
    }

    moveHistory.pop_back();
    return true;
}

bool ChessBoard::redo_move() {
    if (moveHistory.empty() || moveHistory.size() == moveHistory.capacity()) return false;

    Move& move = moveHistory[moveHistory.size()];

    // Redo the move
    board[move.destRow][move.destCol] = board[move.srcRow][move.srcCol];
    board[move.srcRow][move.srcCol] = { EMPTY, NONE };

    // Handle special cases
    if (move.wasEnPassant) {
        int capturedPawnRow = (move.srcRow + move.destRow) / 2;
        board[capturedPawnRow][move.destCol] = { EMPTY, NONE };
    } else if (move.wasCastling) {
        int rookSrcCol = (move.destCol > move.srcCol) ? 7 : 0;
        int rookDestCol = (move.destCol > move.srcCol) ? move.destCol - 1 : move.destCol + 1;
        board[move.srcRow][rookDestCol] = board[move.srcRow][rookSrcCol];
        board[move.srcRow][rookSrcCol] = { EMPTY, NONE };
    } else if (move.wasPawnPromotion) {
        board[move.destRow][move.destCol].type = QUEEN; // Assuming promotion is always to queen
    }

    // Update castling flags
    if (move.srcRow == 0 && move.srcCol == 4) blackKingMoved = true;
    if (move.srcRow == 7 && move.srcCol == 4) whiteKingMoved = true;
    if (move.srcRow == 0 && move.srcCol == 0) blackQueensideRookMoved = true;
    if (move.srcRow == 0 && move.srcCol == 7) blackKingsideRookMoved = true;
    if (move.srcRow == 7 && move.srcCol == 0) whiteQueensideRookMoved = true;
    if (move.srcRow == 7 && move.srcCol == 7) whiteKingsideRookMoved = true;

    // Update last move information
    lastMoveStart = {move.srcRow, move.srcCol};
    lastMove = {move.destRow, move.destCol};
    lastMoveWasPawnDouble = (board[move.destRow][move.destCol].type == PAWN && std::abs(move.destRow - move.srcRow) == 2);

    moveHistory.push_back(move);
    return true;
}

std::string ChessBoard::move_to_string(const Move& move) const {
    char cols[] = "abcdefgh";
    std::string pieceStr = piece_to_string(board[move.destRow][move.destCol].type);
    return pieceStr + cols[move.srcCol] + std::to_string(8 - move.srcRow) +
           " to " + cols[move.destCol] + std::to_string(8 - move.destRow);
}

std::vector<std::string> ChessBoard::get_move_history_strings() const {
    std::vector<std::string> history;
    for (const auto& move : moveHistory) {
        history.push_back(move_to_string(move));
    }
    return history;
}
