#pragma once

#include <MY_Scene_Base.h>

class PointLight;

class MY_Scene_SurfaceShaders : public MY_Scene_Base{
public:
	ComponentShaderBase * diffuseShader;
	ComponentShaderBase * phongShader;
	ComponentShaderBase * blinnShader;
	ComponentShaderBase * toonShader;

	std::vector<Shader *> shaders;
	
	// We need a light source, otherwise most of the objects in the scene will be black
	PointLight * light;

	virtual void update(Step * _step) override;

	MY_Scene_SurfaceShaders(Game * _game);
	~MY_Scene_SurfaceShaders();
};