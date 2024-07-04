#include <SDL2/SDL.h>
#include <map>
#include "graphics.h"
#include "board.h"
#include "piece.h"

/** Function to initalize SDL (Simple DirectMedia Layer) **/
bool init(SDL_Window** window, SDL_Renderer** renderer) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		return false;
	}

	*window = SDL_CreateWindow("Chess Game!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 640, SDL_WINDOW_SHOWN);
	if(!*window) {
		printf("Error in creating window: %s\n", SDL_GetError());
		return false;
	}

	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if(!*renderer) {
		printf("Error in creating renderer: %s\n", SDL_GetError());
		return false;
	}
	return true;
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

/** Main Function **/
int main(int argc, char*args[]) {
	/* Initialize SDL */
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	if(!init(&window, &renderer)) {
		printf("Falied to initialize window...\n");
		return -1;
	}

	/* Initialize Textures and ChessBoard */
	std::map<PieceType, SDL_Texture*> white_pieces;
	std::map<PieceType, SDL_Texture*> black_pieces;
	load_piece_textures(renderer, white_pieces, black_pieces);

	ChessBoard chessBoard;
	bool quit = false;
	SDL_Event e;

	/* While the User has not Quit the game... */
	while(!quit) {
		while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT) {
				quit = true;
			}
		}
		/* Draw and render pieces */
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		draw_board(renderer);
		draw_pieces(renderer, chessBoard, white_pieces, black_pieces);
		SDL_RenderPresent(renderer);
	}
	
	/* Destroy Textures, Renderer, and Window after breaking out of loop */
	for(auto& texture : white_pieces) {
		SDL_DestroyTexture(texture.second);
	}
	for(auto& texture : black_pieces) {
		SDL_DestroyTexture(texture.second);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
