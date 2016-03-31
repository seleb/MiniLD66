#pragma once

#include <MY_Scene_Base.h>

class BulletWorld;
class BulletDebugDrawer;
class DirectionalLight;
class Unit;

class MapCell : public Node{
public:
	MapCell(glm::vec3 _position = glm::vec3(0));
	Unit * unit;
	glm::vec3 position;
};

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
	Unit * selectedUnit;
	
	std::map<NodeBulletBody *, MapCell *> colliderToCell;
	std::map<std::pair<int, std::pair<int, int>>, MapCell *> positionToCell;
	MapCell *& getCellFromPosition(glm::vec3 _position);


	MeshEntity * cellHighlight;

	OrthographicCamera * gameCam;
	std::vector<Unit *> units;

	virtual void update(Step * _step) override;

	MY_Scene_Main(Game * _game);
	~MY_Scene_Main();
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;
};