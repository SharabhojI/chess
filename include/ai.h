/** Header File declaring AI Bot Functions **/
#ifndef AI_H
#define AI_H

#include <cstddef>
#include "board.h"

int evaluate_board(const ChessBoard& board);
std::vector<std::pair<int, int>> generate_moves(const ChessBoard& board, int row, int col);
int minimax(ChessBoard& board, int depth, bool isMaximizingPlayer, int alpha, int beta, int& moveCount);
void make_best_move(ChessBoard& board);

#endif // AI_H
