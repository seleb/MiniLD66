#pragma once

#include <MY_Scene_Base.h>

class BulletMeshEntity;

class MY_Scene_Bullet3D : public MY_Scene_Base{
public:
	// The scene's physics world
	BulletWorld * bulletWorld;
	// used to draw wireframes showing physics colliders, transforms, etc
	BulletDebugDrawer * bulletDebugDrawer;
	
	// A BulletMeshEntity is a class which combines a Bullet physics node and a standard MeshEntity.
	// The physics node alters the position/orientation of the MeshEntity's childTransform
	// during the update loop to match the physics body's position/orientation within the Bullet world
	//
	// In this scene, we are using a BulletMeshEntity to represent a player object
	// In order to control the player object, we will be treating it as a rigid body and applying forces/impulses
	BulletMeshEntity * player;
	// In this scene, we are using a BulletMeshEntity to represent a ground object
	// This object will be a static body, and has an infinite plane as a collider
	BulletMeshEntity * ground;

	MY_Scene_Bullet3D(Game * _game);
	~MY_Scene_Bullet3D();


	virtual void update(Step * _step) override;
	
	// overriden to add physics debug drawing
	virtual void enableDebug() override;
	// overriden to remove physics debug drawing
	virtual void disableDebug() override;
};