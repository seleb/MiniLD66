#pragma once

#include <MY_Scene_Base.h>

class BulletWorld;
class BulletDebugDrawer;
class DirectionalLight;

class MY_Scene_Main : public MY_Scene_Base{
public:
	// The scene's physics world
	BulletWorld * bulletWorld;
	// used to draw wireframes showing physics colliders, transforms, etc
	BulletDebugDrawer * bulletDebugDrawer;


	ComponentShaderBase * diffuseShader;

	std::vector<Shader *> shaders;
	
	DirectionalLight * sun;
	NodeBulletBody * selectedCell;
	std::map<NodeBulletBody *, glm::vec3> cellGrid;

	MeshEntity * cellHighlight;

	OrthographicCamera * gameCam;

	virtual void update(Step * _step) override;

	MY_Scene_Main(Game * _game);
	~MY_Scene_Main();
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;
};