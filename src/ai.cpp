#include "ai.h"
#include "board.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>

const int NUM_TILES = 8;
const int ALPHA_INITIAL = std::numeric_limits<int>::min();
const int BETA_INITIAL = std::numeric_limits<int>::max();
const int SEARCH_DEPTH = 3; // Define the search depth as a constant or a variable that can be adjusted
const int MAX_MOVES = 100; // To prevent infinite loops

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

/** Function to generate moves for a piece at (row, col) **/
std::vector<std::pair<int, int>> generate_moves(const ChessBoard& board, int row, int col) {
    std::vector<std::pair<int, int>> moves;
    for (int destRow = 0; destRow < NUM_TILES; destRow++) {
        for (int destCol = 0; destCol < NUM_TILES; destCol++) {
            if (is_valid_move(board, row, col, destRow, destCol, board[row][col].color)) {
                moves.push_back({destRow, destCol});
            }
        }
    }
    return moves;
}

/** Minimax with Alpha-Beta Pruning Algorithm **/
int minimax(ChessBoard& board, int depth, bool isMaximizingPlayer, int alpha, int beta, int& moveCount) {
    if (depth == 0 || moveCount > MAX_MOVES) return evaluate_board(board);

    std::vector<std::pair<int, int>> allMoves;
    for (int row = 0; row < NUM_TILES; row++) {
        for (int col = 0; col < NUM_TILES; col++) {
            if (board.board[row][col].color == (isMaximizingPlayer ? WHITE : BLACK)) {
                auto moves = generate_moves(board, row, col);
                for (const auto& move : moves) {
                    allMoves.push_back({row * NUM_TILES + col, move.first * NUM_TILES + move.second});
                }
            }
        }
    }

    if (allMoves.empty()) {
        // If no moves are available, it's either checkmate or stalemate
        if (is_check(board, isMaximizingPlayer ? WHITE : BLACK)) {
            return isMaximizingPlayer ? ALPHA_INITIAL : BETA_INITIAL;
        } else {
            return 0; // Stalemate
        }
    }

    if (isMaximizingPlayer) {
        int maxEval = ALPHA_INITIAL;
        for (const auto& move : allMoves) {
            int srcRow = move.first / NUM_TILES, srcCol = move.first % NUM_TILES;
            int destRow = move.second / NUM_TILES, destCol = move.second % NUM_TILES;

            ChessBoard tempBoard = board;
            tempBoard.board[destRow][destCol] = tempBoard.board[srcRow][srcCol];
            tempBoard.board[srcRow][srcCol] = { EMPTY, NONE };

            moveCount++;
            int eval = minimax(tempBoard, depth - 1, false, alpha, beta, moveCount);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = BETA_INITIAL;
        for (const auto& move : allMoves) {
            int srcRow = move.first / NUM_TILES, srcCol = move.first % NUM_TILES;
            int destRow = move.second / NUM_TILES, destCol = move.second % NUM_TILES;

            ChessBoard tempBoard = board;
            tempBoard.board[destRow][destCol] = tempBoard.board[srcRow][srcCol];
            tempBoard.board[srcRow][srcCol] = { EMPTY, NONE };

            moveCount++;
            int eval = minimax(tempBoard, depth - 1, true, alpha, beta, moveCount);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

/** Function to Make the Best Move **/
void make_best_move(ChessBoard& board) {
    std::cout << "AI is selecting a move" << std::endl;
    int bestEval = ALPHA_INITIAL;
    std::pair<int, int> bestMoveFrom = {-1, -1};
    std::pair<int, int> bestMoveTo = {-1, -1};
    int moveCount = 0;

    for (int row = 0; row < NUM_TILES; row++) {
        for (int col = 0; col < NUM_TILES; col++) {
            if (board.board[row][col].color == WHITE) {
                std::vector<std::pair<int, int>> moves = generate_moves(board, row, col);
                for (const auto& move : moves) {
                    ChessBoard tempBoard = board;
                    tempBoard.board[move.first][move.second] = tempBoard.board[row][col];
                    tempBoard.board[row][col] = { EMPTY, NONE };
                    int eval = minimax(tempBoard, SEARCH_DEPTH, false, ALPHA_INITIAL, BETA_INITIAL, moveCount);
                    if (eval > bestEval) {
                        bestEval = eval;
                        bestMoveFrom = {row, col};
                        bestMoveTo = move;
                    }
                }
            }
        }
    }

    if (bestMoveFrom.first != -1 && bestMoveTo.first != -1) {
        std::cout << "AI selected move from (" << bestMoveFrom.first << "," << bestMoveFrom.second
                  << ") to (" << bestMoveTo.first << "," << bestMoveTo.second << ")" << std::endl;

        board.board[bestMoveTo.first][bestMoveTo.second] = board.board[bestMoveFrom.first][bestMoveFrom.second];
        board.board[bestMoveFrom.first][bestMoveFrom.second] = { EMPTY, NONE };
    } else {
        std::cout << "AI couldn't find a valid move!" << std::endl;
    }
}
