# AI Chess Game

This project is an implementation of a chess game with an AI opponent using C++ and SDL2. It is meant as an experiential learning project in game development, AI algorithms, and graphics programming.

## Features

- Graphical user interface using SDL2
- Player vs AI gameplay
- Legal move validation for all chess pieces
- Special move implementations:
  - Castling
  - En passant
  - Pawn promotion
- Check and checkmate detection
- Stalemate detection
- Move history display

## AI Algorithm

The AI opponent uses the Minimax algorithm with Alpha-Beta pruning to make decisions. The current implementation includes:

- Depth-limited search (fixed to depth of 3)
- Basic position evaluation
- Move generation for all pieces

## Dependencies

- C++11 or higher
- SDL2
- SDL2_image
- SDL2_ttf

## Building and Running

To build and run the project, you need to have the required dependencies installed on your system.

1. Clone the repository:
   ```
   git clone https://github.com/SharabhojI/chess.git
   cd chess
   ```

2. Build the project:
   ```
   make
   ```

3. Run the game:
   ```
   make run
   ```

## Controls

- Click the "Play" button to start a new game (AI makes first move)
- Click on a piece to select it
- Click on a highlighted square to move the selected piece
- The AI will automatically make its move after the player's turn

## Future Improvements

- Implement an opening book for the AI
- Add difficulty levels
- Implement a graphical menu system
- Option to swap color

## Acknowledgments

- [SDL2](https://www.libsdl.org/) for providing the graphics library
- Chess programming community for algorithm insights and best practices
- Chess piece images are from Wikimedia Commons, specifically from the [PNG chess pieces/Standard transparent](https://commons.wikimedia.org/wiki/Category:PNG_chess_pieces/Standard_transparent) category. These images are available under the Creative Commons Attribution-Share Alike 3.0 Unported license.

## Image Credits

The chess piece images used in this project are sourced from Wikimedia Commons:

- Source: [PNG chess pieces/Standard transparent](https://commons.wikimedia.org/wiki/Category:PNG_chess_pieces/Standard_transparent)
- License: [Creative Commons Attribution-Share Alike 3.0 Unported](https://creativecommons.org/licenses/by-sa/3.0/deed.en)
- No changes were made to the original images

I am grateful to the Wikimedia Commons community for providing these high-quality, freely-licensed chess piece images.
