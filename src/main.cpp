/** Includes **/
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <thread>
#include "graphics.h"
#include "board.h"
#include "piece.h"
#include "ai.h"

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

void update_move_history(int srcRow, int srcCol, int destRow, int destCol, Piece piece) {
    char cols[] = "abcdefgh";
    std::string move = piece_to_string(piece.type) + cols[srcCol] + std::to_string(8 - srcRow) + " to " + cols[destCol] + std::to_string(8 - destRow);
    moveHistory.push_back(move);
}

void make_move(ChessBoard& board, int srcRow, int srcCol, int destRow, int destCol) {
    Piece piece = board.board[srcRow][srcCol];
    update_move_history(srcRow, srcCol, destRow, destCol, piece);

    board.board[destRow][destCol] = piece;
    board.board[srcRow][srcCol] = { EMPTY, NONE };
    make_best_move(board);
}

std::vector<std::pair<int, int>> get_valid_moves(const ChessBoard& board, int row, int col) {
    std::vector<std::pair<int, int>> valid_moves;
    for (int destRow = 0; destRow < 8; destRow++) {
        for (int destCol = 0; destCol < 8; destCol++) {
            if (is_valid_move(board, row, col, destRow, destCol)) {
                valid_moves.push_back({destRow, destCol});
            }
        }
    }
    return valid_moves;
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
    bool pieceSelected = false;
    int selectedRow = -1, selectedCol = -1;
    std::vector<std::pair<int, int>> valid_moves;

    bool game_started = false;

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
                    make_best_move(chessBoard);
                } else if (is_inside_button(x, y, reset_button_x, reset_button_y, button_width, button_height)) {
                    chessBoard = ChessBoard();
                    game_started = false;
                    moveHistory.clear();
                } else if (game_started) {
                    int row = y / 80;
                    int col = x / 80;

                    if (pieceSelected) {
                        if (is_valid_move(chessBoard, selectedRow, selectedCol, row, col)) {
                            make_move(chessBoard, selectedRow, selectedCol, row, col);
                        }
                        pieceSelected = false;
                        selectedRow = -1;
                        selectedCol = -1;
                        valid_moves.clear();
                    } else {
                        if (chessBoard.board[row][col].type != EMPTY && chessBoard.board[row][col].color == BLACK) {
                            selectedRow = row;
                            selectedCol = col;
                            pieceSelected = true;
                            valid_moves = get_valid_moves(chessBoard, row, col);
                        }
                    }
                }
            }
        }

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
