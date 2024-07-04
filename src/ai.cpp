#include "ai.h"
#include <algorithm>
#include <vector>

const int NUM_TILES = 8;

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

/** Function to Determine if a move is Valid **/
bool is_valid_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    // @TODO: Implement proper move validation
    return true; // For simplicity, assume all moves are valid
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

/** Minimax Algorithm **/
int minimax(ChessBoard& board, int depth, bool isMaximizingPlayer) {
    if (depth == 0) return evaluate_board(board);

    if (isMaximizingPlayer) {
        int maxEval = -10000;
        for (int row = 0; row < NUM_TILES; row++) {
            for (int col = 0; col < NUM_TILES; col++) {
                if (board.board[row][col].color == WHITE) {
                    std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                    for (const auto& move : moves) {
                        ChessBoard tempBoard = board;
                        // Make the move
                        tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                        tempBoard.board[row][col] = { EMPTY, NONE };
                        int eval = minimax(tempBoard, depth - 1, false);
                        maxEval = std::max(maxEval, eval);
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = 10000;
        for (int row = 0; row < NUM_TILES; row++) {
            for (int col = 0; col < NUM_TILES; col++) {
                if (board.board[row][col].color == BLACK) {
                    std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                    for (const auto& move : moves) {
                        ChessBoard tempBoard = board;
                        // Make the move
                        tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                        tempBoard.board[row][col] = { EMPTY, NONE };
                        int eval = minimax(tempBoard, depth - 1, true);
                        minEval = std::min(minEval, eval);
                    }
                }
            }
        }
        return minEval;
    }
}

/** Alpha-Beta Pruning Algorithm **/
int alpha_beta(ChessBoard& board, int depth, int alpha, int beta, bool isMaximizingPlayer) {
    if (depth == 0) return evaluate_board(board);

    if (isMaximizingPlayer) {
        int maxEval = -10000;
        for (int row = 0; row < NUM_TILES; row++) {
            for (int col = 0; col < NUM_TILES; col++) {
                if (board.board[row][col].color == WHITE) {
                    std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                    for (const auto& move : moves) {
                        ChessBoard tempBoard = board;
                        // Make the move
                        tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                        tempBoard.board[row][col] = { EMPTY, NONE };
                        int eval = alpha_beta(tempBoard, depth - 1, alpha, beta, false);
                        maxEval = std::max(maxEval, eval);
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha) break;
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = 10000;
        for (int row = 0; row < NUM_TILES; row++) {
            for (int col = 0; col < NUM_TILES; col++) {
                if (board.board[row][col].color == BLACK) {
                    std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                    for (const auto& move : moves) {
                        ChessBoard tempBoard = board;
                        // Make the move
                        tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                        tempBoard.board[row][col] = { EMPTY, NONE };
                        int eval = alpha_beta(tempBoard, depth - 1, alpha, beta, true);
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
    int bestEval = -10000;
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
                    int eval = alpha_beta(tempBoard, 3, -10000, 10000, false);
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
