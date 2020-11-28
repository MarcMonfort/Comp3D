#ifndef _GAMESTATE_INCLUDE
#define _GAMESTATE_INCLUDE


class GameState
{

public:
	virtual void init() = 0;
	virtual void update(int deltaTime) = 0;
	virtual void render() = 0;

	bool getKey(int key) const;
	bool getSpecialKey(int key) const;

protected:
	bool keys[256], specialKeys[256]; // Store key states so that 
	void resetKeys();
};

#endif
