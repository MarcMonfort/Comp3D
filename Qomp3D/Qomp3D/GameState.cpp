#include "GameState.h"


bool GameState::getKey(int key) const
{
	return keys[key];
}

bool GameState::getSpecialKey(int key) const
{
	return specialKeys[key];
}

void GameState::resetKeys()
{
	for (int i = 0; i < sizeof(keys); ++i)
	{
		keys[i] = false;
	}
	for (int i = 0; i < sizeof(specialKeys); ++i)
	{
		specialKeys[i] = false;
	}
}
