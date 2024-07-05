#include "ai.h"
#include <algorithm>
#include <vector>

const int NUM_TILES = 8;
const int ALPHA_INITIAL = -10000;
const int BETA_INITIAL = 10000;
const int SEARCH_DEPTH = 3; // Define the search depth as a constant or a variable that can be adjusted

/** Score Evaluation Function **/
int evaluate_board(const ChessBoard& board) {
    int score = 0;
    for (const auto& row : board.board) {
        for (const auto& piece : row) {
            if (piece.color == WHITE) {
                if (piece.type == PAWN) score += 1;
                if (piece.type == KNIGHT || piece.type == BISHOP) score += 3;
                if (piece.type == ROOK) score += 5;
                if (piece.type == QUEEN) score += 9;
            } else if (piece.color == BLACK) {
                if (piece.type == PAWN) score -= 1;
                if (piece.type == KNIGHT || piece.type == BISHOP) score -= 3;
                if (piece.type == ROOK) score -= 5;
                if (piece.type == QUEEN) score -= 9;
            }
        }
    }
    return score;
}

bool is_valid_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    Piece srcPiece = board.board[srcRow][srcCol];
    Piece destPiece = board.board[destRow][destCol];

    // Check if the move is within bounds
    if (destRow < 0 || destRow >= NUM_TILES || destCol < 0 || destCol >= NUM_TILES) {
        return false;
    }

    // Prevent capturing own pieces
    if (srcPiece.color == destPiece.color) {
        return false;
    }

    switch (srcPiece.type) {
        case PAWN:
            if (srcPiece.color == WHITE) {
                // Standard one square forward move
                if (destRow == srcRow - 1 && destCol == srcCol && destPiece.type == EMPTY) {
                    return true;
                }
                // Initial two squares forward move
                if (srcRow == 6 && destRow == srcRow - 2 && destCol == srcCol && destPiece.type == EMPTY) {
                    return true;
                }
                // Diagonal capture
                if (destRow == srcRow - 1 && abs(destCol - srcCol) == 1 && destPiece.color == BLACK) {
                    return true;
                }
            } else if (srcPiece.color == BLACK) {
                // Standard one square forward move
                if (destRow == srcRow + 1 && destCol == srcCol && destPiece.type == EMPTY) {
                    return true;
                }
                // Initial two squares forward move
                if (srcRow == 1 && destRow == srcRow + 2 && destCol == srcCol && destPiece.type == EMPTY) {
                    return true;
                }
                // Diagonal capture
                if (destRow == srcRow + 1 && abs(destCol - srcCol) == 1 && destPiece.color == WHITE) {
                    return true;
                }
            }
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

/** Function to generate moves for a piece at (row, col) **/
std::vector<std::pair<int, int>> generate_moves(const ChessBoard& board, int row, int col) {
    std::vector<std::pair<int, int>> moves;
    for (int destRow = 0; destRow < NUM_TILES; destRow++) {
        for (int destCol = 0; destCol < NUM_TILES; destCol++) {
            if (is_valid_move(board, row, col, destRow, destCol)) {
                moves.push_back({destRow, destCol});
            }
        }
    }
    return moves;
}

/** Minimax with Alpha-Beta Pruning Algorithm **/
int minimax(ChessBoard& board, int depth, bool isMaximizingPlayer, int alpha = ALPHA_INITIAL, int beta = BETA_INITIAL) {
    if (depth == 0) return evaluate_board(board);

    if (isMaximizingPlayer) {
        int maxEval = ALPHA_INITIAL;
        for (int row = 0; row < NUM_TILES; row++) {
            for (int col = 0; col < NUM_TILES; col++) {
                if (board.board[row][col].color == WHITE) {
                    std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                    for (const auto& move : moves) {
                        ChessBoard tempBoard = board;
                        // Make the move
                        tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                        tempBoard.board[row][col] = { EMPTY, NONE };
                        int eval = minimax(tempBoard, depth - 1, false, alpha, beta);
                        maxEval = std::max(maxEval, eval);
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha) break;
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = BETA_INITIAL;
        for (int row = 0; row < NUM_TILES; row++) {
            for (int col = 0; col < NUM_TILES; col++) {
                if (board.board[row][col].color == BLACK) {
                    std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                    for (const auto& move : moves) {
                        ChessBoard tempBoard = board;
                        // Make the move
                        tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                        tempBoard.board[row][col] = { EMPTY, NONE };
                        int eval = minimax(tempBoard, depth - 1, true, alpha, beta);
                        minEval = std::min(minEval, eval);
                        beta = std::min(beta, eval);
                        if (beta <= alpha) break;
                    }
                }
            }
        }
        return minEval;
    }
}

/** Function to Make the Best Move **/
void make_best_move(ChessBoard& board) {
    int bestEval = ALPHA_INITIAL;
    std::pair<int, int> bestMoveFrom;
    std::pair<int, int> bestMoveTo;
    for (int row = 0; row < NUM_TILES; row++) {
        for (int col = 0; col < NUM_TILES; col++) {
            if (board.board[row][col].color == WHITE) {
                std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                for (const auto& move : moves) {
                    ChessBoard tempBoard = board;
                    // Make the move
                    tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                    tempBoard.board[row][col] = { EMPTY, NONE };
                    int eval = minimax(tempBoard, SEARCH_DEPTH, false, ALPHA_INITIAL, BETA_INITIAL);
                    if (eval > bestEval) {
                        bestEval = eval;
                        bestMoveFrom = {row, col};
                        bestMoveTo = move;
                    }
                }
            }
        }
    }
    // Make the best move
    board.board[bestMoveTo.first][bestMoveTo.second] = board.board[bestMoveFrom.first][bestMoveFrom.second];
    board.board[bestMoveFrom.first][bestMoveFrom.second] = { EMPTY, NONE };
}
