#pragma once

#include <MY_Scene_Base.h>

class DirectionalLight;

class MY_Scene_Main : public MY_Scene_Base{
public:
	ComponentShaderBase * diffuseShader;

	std::vector<Shader *> shaders;
	
	DirectionalLight * sun;

	OrthographicCamera * gameCam;

	virtual void update(Step * _step) override;

	MY_Scene_Main(Game * _game);
	~MY_Scene_Main();
};