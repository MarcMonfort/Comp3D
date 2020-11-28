#ifndef _GAME_INCLUDE
#define _GAME_INCLUDE


#include "GameState.h"
#include "MenuGameState.h"
#include "PlayGameState.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720


// Game is a singleton (a class with a single instance) that represents our whole application


class Game
{

public:
	Game() {}
	
	
	static Game &instance()
	{
		static Game G;
	
		return G;
	}
	
	void init();
	bool update(int deltaTime);
	void render();
	
	// Input callback methods
	void keyPressed(int key);
	void keyReleased(int key);
	void specialKeyPressed(int key);
	void specialKeyReleased(int key);
	void mouseMove(int x, int y);
	void mousePress(int button);
	void mouseRelease(int button);

	//void reshape(int width, int height);
	
	bool getKey(int key) const;
	bool getSpecialKey(int key) const;

	void startGame();
	void goBackToMenu();

	void setBplay(bool b);

private:
	bool bPlay;                       // Continue to play game?
	bool keys[256], specialKeys[256]; // Store key states so that we can have access at any time

	GameState* currentGameState;
};


#endif // _GAME_INCLUDE
