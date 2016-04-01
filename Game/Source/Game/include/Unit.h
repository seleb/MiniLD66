#pragma once

#include <MeshEntity.h>

class MapCell;
class Timeout;

class Unit : public MeshEntity{
public:
	glm::vec3 currentPosition;
	glm::vec3 targetPosition;

	MapCell * cell;

	// which team this unit is aligned with
	int team;

	float power;
	bool canAttack;
	bool canMove;

	Timeout * moveTimeout, * attackTimeout, * wanderTimeout, * waitTimeout;

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
};