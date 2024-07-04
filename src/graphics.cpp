#include "graphics.h"
#include <SDL2/SDL_image.h>
#include <iostream>

const int NUM_TILES = 8;
const int TILE_SIZE = 80;

void draw_board(SDL_Renderer* renderer) {
    // Drawing a simple chess board (8x8 grid)
    bool white = true;

    for (int y = 0; y < NUM_TILES; y++) {
        for (int x = 0; x < NUM_TILES; x++) {
            SDL_Rect tile = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if (white) {
                SDL_SetRenderDrawColor(renderer, 245, 245, 220, 255); // Beige color for 'white' tiles
            } else {
                SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
            }
            SDL_RenderFillRect(renderer, &tile);
            white = !white;
        }
        white = !white;
    }
}

void draw_pieces(SDL_Renderer* renderer, ChessBoard& chessBoard, std::map<PieceType, SDL_Texture*>& white_pieces, std::map<PieceType, SDL_Texture*>& black_pieces) {
    for (int y = 0; y < NUM_TILES; y++) {
        for (int x = 0; x < NUM_TILES; x++) {
            Piece piece = chessBoard.board[y][x];
            SDL_Rect dest = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if (piece.color == WHITE) {
                SDL_RenderCopy(renderer, white_pieces[piece.type], NULL, &dest);
            } else if (piece.color == BLACK) {
                SDL_RenderCopy(renderer, black_pieces[piece.type], NULL, &dest);
            }
        }
    }
}

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(file);
    if (loadedSurface == NULL) {
        std::cout << "Unable to load image " << file << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL) {
            std::cout << "Unable to create texture from " << file << "! SDL Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}
