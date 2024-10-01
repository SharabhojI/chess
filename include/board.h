/** Header File declaring class ChessBoard **/
#ifndef BOARD_H // If not defined
#define BOARD_H // Define

#include <vector>
#include <string>
#include "piece.h"

class ChessBoard {
public:
    ChessBoard();
    void setup_pieces();
    std::vector<std::vector<Piece>> board;

    // For en passant tracking
    std::pair<int, int> lastMove = {-1, -1};
    std::pair<int, int> lastMoveStart = {-1, -1};
    bool lastMoveWasPawnDouble = false;

    // For tracking castling eligibility
    bool whiteKingMoved;
    bool blackKingMoved;
    bool whiteKingsideRookMoved;
    bool whiteQueensideRookMoved;
    bool blackKingsideRookMoved;
    bool blackQueensideRookMoved;

    const std::vector<Piece>& operator[](int index) const {
        return board[index];
    }

    std::vector<Piece>& operator[](int index) {
        return board[index];
    }

    struct Move {
        int srcRow, srcCol, destRow, destCol;
        Piece capturedPiece;
        bool wasEnPassant;
        bool wasCastling;
        bool wasPawnPromotion;
        PieceType promotedTo;

        Move(int sr, int sc, int dr, int dc, Piece captured, bool ep = false, bool castle = false, bool promotion = false, PieceType promo = EMPTY)
            : srcRow(sr), srcCol(sc), destRow(dr), destCol(dc), capturedPiece(captured),
              wasEnPassant(ep), wasCastling(castle), wasPawnPromotion(promotion), promotedTo(promo) {}
    };

    std::vector<Move> moveHistory;
    void record_move(int srcRow, int srcCol, int destRow, int destCol);
    bool undo_last_move();
    bool redo_move();
    std::string move_to_string(const Move& move) const;
    std::vector<std::string> get_move_history_strings() const;
};

// Piece movement validity
bool would_be_in_check(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);
bool is_valid_pawn_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);
bool is_valid_knight_move(int srcRow, int srcCol, int destRow, int destCol);
bool is_valid_bishop_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);
bool is_valid_rook_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);
bool is_valid_queen_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);
bool is_valid_king_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol);

bool is_valid_move(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol, Color currentTurn);
std::vector<std::pair<int, int>> get_valid_moves(const ChessBoard& board, int row, int col, Color currentTurn);
bool is_check(const ChessBoard& board, Color color);
bool is_checkmate(ChessBoard& board, Color color);
bool is_stalemate(ChessBoard& board, Color color);

// Function declarations for castling logic
bool is_castling_valid(const ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol, Color color);
bool is_square_attacked(const ChessBoard& board, int row, int col, Color attackingColor);

std::string piece_to_string(PieceType type);

#endif // BOARD_H
