/** Header File declaring AI Bot Functions **/
#ifndef AI_H
#define AI_H

#include "board.h"

int evaluate_board(const ChessBoard& board);
int minimax(ChessBoard& board, int depth, bool isMaximizingPlayer);
int alpha_beta(ChessBoard& board, int depth, int alpha, int beta, bool isMaximizingPlayer);
void make_best_move(ChessBoard& board);

#endif // AI_H
