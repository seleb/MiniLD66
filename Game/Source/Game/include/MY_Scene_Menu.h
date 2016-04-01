#pragma once

#include <MY_Scene_Base.h>

class MY_Scene_Menu : public MY_Scene_Base{
public:
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	float colTime;


	MY_Scene_Menu(Game * _game);
	~MY_Scene_Menu();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};