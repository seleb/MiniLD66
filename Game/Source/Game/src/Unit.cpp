#pragma once

#include <Unit.h>
#include <MY_ResourceManager.h>
#include <Timeout.h>
#include <MY_Scene_Main.h>
#include <NumberUtils.h>
#include <shader/ShaderComponentTint.h>
#include <Easing.h>

Unit::Unit(int _team, glm::vec3 _position, Shader * _shader) :
	MeshEntity(MY_ResourceManager::globalAssets->getMesh(_team == 0 ? "UNIT-GOOD" : "UNIT-BAD")->meshes.at(0), _shader),
	currentPosition(_position),
	targetPosition(_position),
	team(_team),
	cell(nullptr),
	canMove(true),
	power(0),
	killTime(0)
{
	//mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("UNIT")->texture);
	mesh->setScaleMode(GL_NEAREST);

	moveTimeout = new Timeout(0.2f, [this](sweet::Event * _event){
		canMove = true;
	});
	childTransform->addChild(moveTimeout, false);

	wanderTimeout = new Timeout(2.5f, [this](sweet::Event * _event){
		waitTimeout->targetSeconds = sweet::NumberUtils::randomFloat(1.5, 10.f);
		waitTimeout->restart();
	});
	childTransform->addChild(wanderTimeout, false);

	waitTimeout = new Timeout(2.5f, [this](sweet::Event * _event){
		if(team == 0){
			targetPosition = glm::vec3(sweet::NumberUtils::randomInt(-MY_Scene_Main::SIZE/2+1, MY_Scene_Main::SIZE/2-2), 0, sweet::NumberUtils::randomInt(-MY_Scene_Main::SIZE/2+1, MY_Scene_Main::SIZE/2-2));
		}else{
			targetPosition = MY_Scene_Main::getRandomUnitPosition();
		}wanderTimeout->targetSeconds = sweet::NumberUtils::randomFloat(1.5, 10.f);
		wanderTimeout->restart();
	});
	childTransform->addChild(waitTimeout, false);
	waitTimeout->start();
	waitTimeout->trigger();

	killTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		killTime = 0;
	});
	killTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		killTime = Easing::easeInCubic(_event->getFloatData("progress"), 1, -1, 1);
	});
	childTransform->addChild(killTimeout, false);
}

Unit::~Unit(){
}

void Unit::update(Step * _step){
	firstParent()->translate(currentPosition, false);
	MeshEntity::update(_step);
}
void Unit::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	float g = tint->getGreen();
	if(team == 1 && killTime > FLT_EPSILON){
		tint->setGreen(-killTime);
		tint->setBlue(-killTime);
	}
	MeshEntity::render(_matrixStack, _renderOptions);
	if(team == 1 && killTime > FLT_EPSILON){
		tint->setGreen(g);
		tint->setBlue(g);
	}
}