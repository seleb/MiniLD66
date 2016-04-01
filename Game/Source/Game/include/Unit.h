#pragma once

#include <MeshEntity.h>

class MapCell;
class Timeout;
class ShaderComponentTint;

class Unit : public MeshEntity{
public:
	ShaderComponentTint * tint;
	float killTime;
	glm::vec3 currentPosition;
	glm::vec3 targetPosition;

	MapCell * cell;

	// which team this unit is aligned with
	int team;

	int id;

	float power;
	bool canMove;

	Timeout * moveTimeout, * wanderTimeout, * waitTimeout, * killTimeout;

	Unit(int _team, glm::vec3 _position, Shader * _shader);
	~Unit();

	// returns a new unit which is a child of this unit and _mate
	Unit * breed(Unit * _mate);
	
	// damages the _target
	// if _target can attack, they will attack back
	// returns nullptr
	void attack(Unit * _target);
	// i'll tell you when you're older
	void mate(Unit * _mate);

	// if _target is friendly, calls mate
	// if _target is not friendly, calls attack
	void interact(Unit * _target);


	virtual void update(Step * _step);
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};