/** Header File for functions rendering Board and Pieces **/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <map>
#include "board.h"

void draw_board(SDL_Renderer* renderer);
void draw_pieces(SDL_Renderer* renderer, ChessBoard& chessBoard, std::map<PieceType, SDL_Texture*>& white_pieces, std::map<PieceType, SDL_Texture*>& black_pieces);
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file);

#endif // GRAPHICS_H
