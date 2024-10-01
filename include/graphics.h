/** Header File for functions rendering Board and Pieces **/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <vector>
#include <string>
#include <cstddef>
#include "board.h"
#include "piece.h"

void draw_board(SDL_Renderer* renderer, int selectedRow, int selectedCol);
void draw_valid_moves(SDL_Renderer* renderer, const std::vector<std::pair<int, int>>& valid_moves);
void draw_pieces(SDL_Renderer* renderer, const ChessBoard& board, const std::map<PieceType, SDL_Texture*>& white_pieces, const std::map<PieceType, SDL_Texture*>& black_pieces);
void draw_button(SDL_Renderer* renderer, TTF_Font* font, int x, int y, int w, int h, const char* text);
void draw_move_history(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const ChessBoard& board);
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file);

#endif // GRAPHICS_H
