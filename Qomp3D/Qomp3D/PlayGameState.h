#ifndef _PLAYGAMESTATE_INCLUDE
#define _PLAYGAMESTATE_INCLUDE

#include "GameState.h"
#include "Scene.h"


class PlayGameState : public GameState
{

public:
	static PlayGameState& instance()
	{
		static PlayGameState P;
		return P;
	}

	// Input callback methods
	void keyPressed(int key);

	void init();
	void update(int deltaTime);
	void render();

	void finalBlockTaken();

	bool getGodMode();
	void setGodMode(bool b);

private:
	Scene* scene;
	int currentLevel = 0;
	bool nextlevel = false;

	void nextLevel();

	bool bGodMode = false;
};

#endif
