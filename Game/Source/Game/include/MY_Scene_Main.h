#pragma once

#include <MY_Scene_Base.h>

class DirectionalLight;

class MY_Scene_Main : public MY_Scene_Base{
public:

	virtual void update(Step * _step) override;

	MY_Scene_Main(Game * _game);
	~MY_Scene_Main();
};
