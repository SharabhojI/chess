/** Header File Defining Data Structs for Chess Pieces **/
#ifndef PIECE_H
#define PIECE_H

enum PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, EMPTY };
enum Color { WHITE, BLACK, NONE };

struct Piece {
	PieceType type;
	Color color;
};

#endif // PIECE_H
