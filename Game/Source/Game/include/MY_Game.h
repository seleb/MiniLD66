#pragma once

#include <Game.h>

class MY_Game : public Game{
public:
	MY_Game();
	~MY_Game();

	void addSplashes() override;
};