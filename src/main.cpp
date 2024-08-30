/** Includes **/
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_messagebox.h>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <thread>
#include <cstddef>
#include <iostream>
#include "graphics.h"
#include "board.h"
#include "piece.h"
#include "ai.h"

const int TILE_SIZE = 80;

bool is_white_turn = true;

/** Data Struct for move history **/
std::vector<std::string> moveHistory;

/** Function to initalize SDL (Simple DirectMedia Layer) **/
bool init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 640, SDL_WINDOW_SHOWN); // Updated width
    if (!*window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
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

/** Function to load Textures for Pieces **/
void load_piece_textures(SDL_Renderer* renderer, std::map<PieceType, SDL_Texture*>& white_pieces, std::map<PieceType, SDL_Texture*>& black_pieces) {
	/* White Pieces */
	white_pieces[PAWN] = load_texture(renderer, "images/white_pawn.png");
	white_pieces[ROOK] = load_texture(renderer, "images/white_rook.png");
	white_pieces[KNIGHT] = load_texture(renderer, "images/white_knight.png");
	white_pieces[BISHOP] = load_texture(renderer, "images/white_bishop.png");
	white_pieces[QUEEN] = load_texture(renderer, "images/white_queen.png");
	white_pieces[KING] = load_texture(renderer, "images/white_king.png");

	/* Black Pieces */
	black_pieces[PAWN] = load_texture(renderer, "images/black_pawn.png");
	black_pieces[ROOK] = load_texture(renderer, "images/black_rook.png");
	black_pieces[KNIGHT] = load_texture(renderer, "images/black_knight.png");
	black_pieces[BISHOP] = load_texture(renderer, "images/black_bishop.png");
	black_pieces[QUEEN] = load_texture(renderer, "images/black_queen.png");
	black_pieces[KING] = load_texture(renderer, "images/black_king.png");
}

bool is_inside_button(int x, int y, int bx, int by, int bw, int bh) {
    return x >= bx && x <= bx + bw && y >= by && y <= by + bh;
}

void reset_game(ChessBoard& board, bool& game_started, bool& is_white_turn, bool& pieceSelected, int& selectedRow, int& selectedCol, std::vector<std::pair<int, int>>& valid_moves) {
    board = ChessBoard();
    game_started = false;
    is_white_turn = true;
    moveHistory.clear();
    pieceSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    valid_moves.clear();
    std::cout << "Game reset!" << std::endl;
}

void show_game_end_message(SDL_Window* window, const char* message, ChessBoard& board, bool& game_started, bool& is_white_turn, bool& pieceSelected, int& selectedRow, int& selectedCol, std::vector<std::pair<int, int>>& valid_moves) {
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "New Game" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Quit" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { 255, 255, 255 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            {   0,   0,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 100, 100, 100 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            { 200, 200, 200 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 150, 150, 150 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        window,
        "Game Over",
        message,
        SDL_arraysize(buttons),
        buttons,
        &colorScheme
    };
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        SDL_Log("error displaying message box");
        return;
    }
    if (buttonid == 0) {
        // Start a new game
        reset_game(board, game_started, is_white_turn, pieceSelected, selectedRow, selectedCol, valid_moves);
    } else {
        // Quit the game
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
    }
}

void update_move_history(int srcRow, int srcCol, int destRow, int destCol, Piece piece) {
    char cols[] = "abcdefgh";
    std::string move = piece_to_string(piece.type) + cols[srcCol] + std::to_string(8 - srcRow) + " to " + cols[destCol] + std::to_string(8 - destRow);
    moveHistory.push_back(move);
}

void make_move(ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    std::cout << "Making move from (" << srcRow << "," << srcCol << ") to (" << destRow << "," << destCol << ")" << std::endl;

    Piece piece = board.board[srcRow][srcCol];
    std::cout << "Moving piece: " << piece.type << ", " << piece.color << std::endl;
    update_move_history(srcRow, srcCol, destRow, destCol, piece);

    // Handle en passant capture
    if (piece.type == PAWN && std::abs(destCol - srcCol) == 1 && board.board[destRow][destCol].type == EMPTY) {
        if (piece.color == WHITE && srcRow == 3) {
            board.board[srcRow][destCol] = { EMPTY, NONE };
        } else if (piece.color == BLACK && srcRow == 4) {
            board.board[srcRow][destCol] = { EMPTY, NONE };
        }
    }

    // Handle castling
    if (piece.type == KING && std::abs(destCol - srcCol) == 2) {
        int rookSrcCol = (destCol > srcCol) ? 7 : 0;
        int rookDestCol = (destCol > srcCol) ? destCol - 1 : destCol + 1;
        board.board[destRow][rookDestCol] = board.board[destRow][rookSrcCol];
        board.board[destRow][rookSrcCol] = { EMPTY, NONE };
    }

    // Update castling flags
    if (piece.type == KING) {
        if (piece.color == WHITE) board.white_king_moved = true;
        else board.black_king_moved = true;
    }
    else if (piece.type == ROOK) {
        if (piece.color == WHITE) {
            if (srcCol == 0) board.white_queenside_rook_moved = true;
            else if (srcCol == 7) board.white_kingside_rook_moved = true;
        }
        else {
            if (srcCol == 0) board.black_queenside_rook_moved = true;
            else if (srcCol == 7) board.black_kingside_rook_moved = true;
        }
    }

    // Pawn Promotion
    if (piece.type == PAWN && (destRow == 0 || destRow == 7)) {
        piece.type = QUEEN;
        std::cout << "Pawn promoted to Queen!" << std::endl;
    }

    // Make the move
    board.board[destRow][destCol] = piece;
    board.board[srcRow][srcCol] = { EMPTY, NONE };

    // Update last move
    board.last_move_start = {srcRow, srcCol};
    board.last_move = {destRow, destCol};
    board.last_move_was_pawn_double = (piece.type == PAWN && std::abs(destRow - srcRow) == 2);

    // Switch turns
    is_white_turn = !is_white_turn;

    // If it's now White's turn, make the AI move
    if (is_white_turn) {
        make_best_move(board);
        is_white_turn = false;  // Switch back to Black's turn after AI moves
    }
}

/** main **/
int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!init(&window, &renderer)) {
        printf("Failed to initialize!\n");
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("fonts/WrittenBirthday.ttf", 24);
    if (font == NULL) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    std::map<PieceType, SDL_Texture*> white_pieces;
    std::map<PieceType, SDL_Texture*> black_pieces;
    load_piece_textures(renderer, white_pieces, black_pieces);

    ChessBoard chessBoard;

    bool quit = false;
    SDL_Event e;
    bool game_started = false;
    bool pieceSelected = false;
    int selectedRow = -1, selectedCol = -1;
    std::vector<std::pair<int, int>> valid_moves;

    int button_width = 100;
    int button_height = 50;
    int play_button_x = 650;
    int play_button_y = 100;
    int reset_button_x = 770;
    int reset_button_y = 100;


    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (is_inside_button(x, y, play_button_x, play_button_y, button_width, button_height)) {
                    game_started = true;
                    is_white_turn = true;  // White (AI) starts
                    std::cout << "Game started!" << std::endl;
                    make_best_move(chessBoard);
                    is_white_turn = false;  // Switch to Black's turn after AI moves
                } else if (is_inside_button(x, y, reset_button_x, reset_button_y, button_width, button_height)) {
                    reset_game(chessBoard, game_started, is_white_turn, pieceSelected, selectedRow, selectedCol, valid_moves);
                } else if (game_started && !is_white_turn) {  // Only allow moves on Black's turn
                    int row = y / TILE_SIZE;
                    int col = x / TILE_SIZE;
                    if (pieceSelected) {
                        if (is_valid_move(chessBoard, selectedRow, selectedCol, row, col, BLACK)) {
                            make_move(chessBoard, selectedRow, selectedCol, row, col);
                            pieceSelected = false;
                            selectedRow = -1;
                            selectedCol = -1;
                            valid_moves.clear();
                        } else {
                            // If the move is invalid, deselect the piece
                            pieceSelected = false;
                            selectedRow = -1;
                            selectedCol = -1;
                            valid_moves.clear();
                        }
                    } else {
                        if (chessBoard.board[row][col].type != EMPTY && chessBoard.board[row][col].color == BLACK) {
                            selectedRow = row;
                            selectedCol = col;
                            pieceSelected = true;
                            valid_moves = get_valid_moves(chessBoard, row, col, BLACK);
                        }
                    }
                    // Check for game-ending conditions
                    if (is_checkmate(chessBoard, WHITE)) {
                        show_game_end_message(window, "Checkmate! Black wins.", chessBoard, game_started, is_white_turn, pieceSelected, selectedRow, selectedCol, valid_moves);
                    } else if (is_checkmate(chessBoard, BLACK))  {
                        show_game_end_message(window, "Checkmate! White wins.", chessBoard, game_started, is_white_turn, pieceSelected, selectedRow, selectedCol, valid_moves);
                    } else if (is_stalemate(chessBoard, WHITE) || is_stalemate(chessBoard, BLACK)) {
                        show_game_end_message(window, "Stalemate! The game is a draw.", chessBoard, game_started, is_white_turn, pieceSelected, selectedRow, selectedCol, valid_moves);
                    }
                }
            }
        }
        // Rendering
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        draw_board(renderer, selectedRow, selectedCol);
        draw_valid_moves(renderer, valid_moves);
        draw_pieces(renderer, chessBoard, white_pieces, black_pieces);
        draw_button(renderer, font, play_button_x, play_button_y, button_width, button_height, "Play");
        draw_button(renderer, font, reset_button_x, reset_button_y, button_width, button_height, "Reset");
        draw_move_history(renderer, font, 650, 200, moveHistory);
        SDL_RenderPresent(renderer);
    }

    for (auto& texture : white_pieces) {
        SDL_DestroyTexture(texture.second);
    }
    for (auto& texture : black_pieces) {
        SDL_DestroyTexture(texture.second);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
