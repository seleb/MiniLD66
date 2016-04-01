#pragma once

#include <Unit.h>
#include <MY_ResourceManager.h>
#include <Timeout.h>


Unit::Unit(int _team, glm::vec3 _position, Shader * _shader) :
	MeshEntity(MY_ResourceManager::globalAssets->getMesh("UNIT")->meshes.at(0), _shader),
	currentPosition(_position),
	targetPosition(_position),
	team(_team),
	cell(nullptr),
	canAttack(true),
	canMove(true)
{
	mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("UNIT")->texture);
	mesh->setScaleMode(GL_NEAREST);

	moveTimeout = new Timeout(0.5f, [this](sweet::Event * _event){
		canMove = true;
	});
	childTransform->addChild(moveTimeout, false);

	attackTimeout = new Timeout(0.5f, [this](sweet::Event * _event){
		canAttack = true;
	});
	childTransform->addChild(attackTimeout, false);
}

Unit::~Unit(){
}

void Unit::update(Step * _step){
	canAttack = true;
	
	firstParent()->translate(currentPosition, false);
	MeshEntity::update(_step);
}

Unit * Unit::breed(Unit * _mate){
	assert(team == _mate->team);

	return nullptr;
}

void Unit::attack(Unit * _target){
	if(canAttack){
		canAttack = false;
		attackTimeout->restart();
		assert(team != _target->team);

		// damage _target

		// if target can attack, let them counter-attack here
		if(_target->canAttack){
			_target->attack(this);
		}
	}
}

void Unit::mate(Unit * _target){
	assert(team == _target->team);
	
}

void Unit::interact(Unit * _target){
	if(team == _target->team){
		mate(_target);
	}else{
		attack(_target);
	}
}