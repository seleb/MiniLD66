#pragma once

#include <MY_Scene_Bullet3D.h>

#include <NodeBulletBody.h>
#include <BulletMeshEntity.h>
#include <BulletWorld.h>

#include <MeshFactory.h>

MY_Scene_Bullet3D::MY_Scene_Bullet3D(Game * _game) :
	MY_Scene_Base(_game),
	bulletWorld(new BulletWorld(glm::vec3(0, -9.8, 0))), // we initialize the world's gravity here
	bulletDebugDrawer(new BulletDebugDrawer(bulletWorld->world))
{
	// Setup the debug drawer and add it to the scene
	bulletWorld->world->setDebugDrawer(bulletDebugDrawer);
	childTransform->addChild(bulletDebugDrawer, false);
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);

	// Setup the ground
	ground = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh(100), baseShader);
	// when we're using a BulletMeshEntity, we need to set a collider and then create a rigid body in order to add the object to the simulation
	ground->setColliderAsStaticPlane(0,1,0);
	ground->createRigidBody(0); // a mass of zero means that it will be a static body instead of a dynamic one
	childTransform->addChild(ground);
	ground->meshTransform->rotate(90, 1, 0, 0, kOBJECT); // the plane is vertical, so we need to rotate it in order to match the collider

	// Setup the player
	player = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(5), baseShader);
	player->setColliderAsBoundingBox();
	player->createRigidBody(1);
	childTransform->addChild(player);
	player->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);

	// when dealing with physics nodes, we use translatePhysical instead of editing the Transform nodes directly
	// this is because we need to inform the physics simulation of the change, not our Transform hierarchy
	// the physics node will handle the placement of its childTransform automatically later during the update loop
	player->translatePhysical(glm::vec3(0, 6, 0), false); 
}

MY_Scene_Bullet3D::~MY_Scene_Bullet3D(){
	// we need to destruct the scene elements before the physics world to avoid memory issues
	deleteChildTransform();
	delete bulletWorld;
}


void MY_Scene_Bullet3D::update(Step * _step){
	// Physics update
	bulletWorld->update(_step);
	// Scene update
	MY_Scene_Base::update(_step);

	// player input
	player->applyLinearImpulseRight(controller->getAxis(controller->axisLeftX)*5);
	if(controller->buttonJustDown(controller->faceButtonDown)){
		player->applyLinearImpulseUp(5);
	}
}

void MY_Scene_Bullet3D::enableDebug(){
	MY_Scene_Base::enableDebug();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
	childTransform->addChildAtIndex(bulletDebugDrawer, -1, false); // make sure the debug drawer is the last thing drawn
}
void MY_Scene_Bullet3D::disableDebug(){
	MY_Scene_Base::disableDebug();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
}