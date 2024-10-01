#include "graphics.h"
#include <SDL2/SDL_image.h>
#include <iostream>

const int NUM_TILES = 8;
const int TILE_SIZE = 80;

void draw_board(SDL_Renderer* renderer, int selectedRow, int selectedCol) {
    for (int row = 0; row < NUM_TILES; ++row) {
        for (int col = 0; col < NUM_TILES; ++col) {
            SDL_Rect tileRect = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if ((row + col) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 245, 245, 220, 255); // Light color (beige)
            } else {
                SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255); // Dark color (grey)
            }
            SDL_RenderFillRect(renderer, &tileRect);

            // Highlight the selected piece
            if (row == selectedRow && col == selectedCol) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Enable blending
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 64); // Yellow highlight 75% transparency
                SDL_RenderFillRect(renderer, &tileRect);
            }
        }
    }
}

void draw_valid_moves(SDL_Renderer* renderer, const std::vector<std::pair<int, int>>& valid_moves) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Enable blending
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 64); // Green highlight 75% transparency
    for (const auto& move : valid_moves) {
        SDL_Rect moveRect = { move.second * TILE_SIZE, move.first * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(renderer, &moveRect);
    }
}

void draw_pieces(SDL_Renderer* renderer, const ChessBoard& board, const std::map<PieceType, SDL_Texture*>& white_pieces, const std::map<PieceType, SDL_Texture*>& black_pieces) {
    for (int row = 0; row < NUM_TILES; ++row) {
        for (int col = 0; col < NUM_TILES; ++col) {
            Piece piece = board.board[row][col];
            if (piece.type != EMPTY) {
                SDL_Rect pieceRect = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                if (piece.color == WHITE) {
                    SDL_RenderCopy(renderer, white_pieces.at(piece.type), NULL, &pieceRect);
                } else {
                    SDL_RenderCopy(renderer, black_pieces.at(piece.type), NULL, &pieceRect);
                }
            }
        }
    }
}

void draw_button(SDL_Renderer* renderer, TTF_Font* font, int x, int y, int w, int h, const char* text) {
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black border
    SDL_RenderDrawRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Gray fill
    SDL_RenderFillRect(renderer, &rect);

    // Render text on the button
    SDL_Color textColor = { 0, 0, 0, 255 }; // Black text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_Rect textRect = { x + (w - text_width) / 2, y + (h - text_height) / 2, text_width, text_height };

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void draw_move_history(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const ChessBoard& board) {
    auto moveHistory = board.get_move_history_strings();
    SDL_Color textColor = { 0, 0, 0, 255 }; // Black text
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, moveHistory[i].c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int text_width = textSurface->w;
        int text_height = textSurface->h;
        SDL_Rect textRect = { x, y + (int)i * text_height, text_width, text_height };

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
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
