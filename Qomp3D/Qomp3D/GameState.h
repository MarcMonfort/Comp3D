#ifndef _GAMESTATE_INCLUDE
#define _GAMESTATE_INCLUDE


class GameState
{

public:
	virtual void init() = 0;
	virtual void update(int deltaTime) = 0;
	virtual void render() = 0;

	// Input callback methods
	virtual void keyPressed(int key) = 0;
};

#endif
