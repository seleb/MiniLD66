#pragma once

#include <MY_Scene_Base.h>

class BulletWorld;
class BulletDebugDrawer;
class DirectionalLight;
class Unit;

#define START_SIZE 6

class MapCell : public Node{
public:
	MapCell(glm::vec3 _position = glm::vec3(0));
	Unit * unit;
	glm::vec3 position;
};

class MY_Scene_Main : public MY_Scene_Base{
public:
	static int SIZE;

	sweet::EventManager * eventManager;


	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;


	// The scene's physics world
	BulletWorld * bulletWorld;
	// used to draw wireframes showing physics colliders, transforms, etc
	BulletDebugDrawer * bulletDebugDrawer;


	ComponentShaderBase * diffuseShader;

	std::vector<Shader *> shaders;
	
	float sunTime;
	DirectionalLight * sun;

	NodeBulletBody * selectedCell;
	Unit * selectedUnit;

	float numBaddies, numUnits, numFriendlies;
	bool gameOver;
	bool gameWon;
	
	std::map<NodeBulletBody *, MapCell *> colliderToCell;
	std::map<std::pair<int, int>, MapCell *> positionToCell;
	MapCell *& getCellFromPosition(glm::vec3 _position);


	MeshEntity * cellHighlight;

	OrthographicCamera * gameCam;
	int camAngle;
	static std::map<int, Unit *> units;
	
	NodeUI * fade;
	Timeout * fadeOutTimer;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	static glm::vec3 getRandomUnitPosition();


	MY_Scene_Main(Game * _game);
	~MY_Scene_Main();
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;

	
	void kill(Unit * _unit);
	void kill(int _unitId);
};