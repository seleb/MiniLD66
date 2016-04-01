#pragma once

#include <Unit.h>
#include <MY_ResourceManager.h>
#include <Timeout.h>
#include <MY_Scene_Main.h>
#include <NumberUtils.h>


Unit::Unit(int _team, glm::vec3 _position, Shader * _shader) :
	MeshEntity(MY_ResourceManager::globalAssets->getMesh(_team == 0 ? "UNIT-GOOD" : "UNIT-BAD")->meshes.at(0), _shader),
	currentPosition(_position),
	targetPosition(_position),
	team(_team),
	cell(nullptr),
	canAttack(true),
	canMove(true),
	power(0)
{
	//mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("UNIT")->texture);
	mesh->setScaleMode(GL_NEAREST);

	moveTimeout = new Timeout(0.2f, [this](sweet::Event * _event){
		canMove = true;
	});
	childTransform->addChild(moveTimeout, false);

	attackTimeout = new Timeout(5.f, [this](sweet::Event * _event){
		canAttack = true;
	});
	childTransform->addChild(attackTimeout, false);

	wanderTimeout = new Timeout(2.5f, [this](sweet::Event * _event){
		waitTimeout->targetSeconds = sweet::NumberUtils::randomFloat(1.5, 10.f);
		waitTimeout->restart();
	});
	childTransform->addChild(wanderTimeout, false);

	waitTimeout = new Timeout(2.5f, [this](sweet::Event * _event){
		targetPosition = glm::vec3(sweet::NumberUtils::randomInt(-SIZE/2+1, SIZE/2-2), 0, sweet::NumberUtils::randomInt(-SIZE/2+1, SIZE/2-2));
		wanderTimeout->targetSeconds = sweet::NumberUtils::randomFloat(1.5, 10.f);
		wanderTimeout->restart();
	});
	childTransform->addChild(waitTimeout, false);
	waitTimeout->start();
	waitTimeout->trigger();
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