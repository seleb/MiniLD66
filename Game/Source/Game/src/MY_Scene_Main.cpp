#pragma once

#include <MY_Scene_Main.h>

#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>

#include <MeshFactory.h>

#include <DirectionalLight.h>
#include <PointLight.h>
#include <NumberUtils.h>
#include <BulletMeshEntity.h>
#include <BulletDebugDrawer.h>
#include <NodeBulletBody.h>

#define SIZE 32

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	diffuseShader(new ComponentShaderBase(true)),
	bulletWorld(new BulletWorld()),
	bulletDebugDrawer(new BulletDebugDrawer(bulletWorld->world)),
	selectedCell(nullptr)
{
	// Setup the debug drawer and add it to the scene
	bulletWorld->world->setDebugDrawer(bulletDebugDrawer);
	childTransform->addChild(bulletDebugDrawer, false);
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);

	diffuseShader->addComponent(new ShaderComponentMVP(diffuseShader));
	diffuseShader->addComponent(new ShaderComponentDiffuse(diffuseShader, false));
	diffuseShader->addComponent(new ShaderComponentTexture(diffuseShader));
	diffuseShader->compileShader();
	diffuseShader->name = "Scene: Main, Shader: diffuse";
	diffuseShader->incrementReferenceCount();

	// SUN
	sun = new DirectionalLight(glm::vec3(1), 0.05f);
	childTransform->addChild(sun);
	lights.push_back(sun);

	PointLight * pl = new PointLight(glm::vec3(0.1f), 0, 0.005f, -1);
	sun->firstParent()->addChild(pl, false);
	lights.push_back(pl);

	// add a cube to the light too so that we can see it without having to use the debug mode
	sun->childTransform->addChild(new MeshEntity(MeshFactory::getCubeMesh(), baseShader),false);


	// TERRAIN
	QuadMesh * cubeMesh = MeshFactory::getCubeMesh();
	Transform t;
	t.translate(0, 0.5f, 0, false);
	cubeMesh->applyTransformation(&t);
	MeshEntity * terrain = new MeshEntity(new QuadMesh(true), diffuseShader);
	childTransform->addChild(terrain);
	for(signed long int x = 0; x < SIZE; ++x){
		for(signed long int y = 0; y < SIZE; ++y){
			glm::vec3 cellPos(x - SIZE/2, 1, y - SIZE/2);
			//MeshEntity * me = new MeshEntity(cubeMesh, diffuseShader);
			//terrain->addChild(me)->translate(x, 0, y)->scale(glm::vec3(1, sweet::NumberUtils::randomInt(1, 3)*0.33f, 1));

			BulletMeshEntity * collider = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), diffuseShader);
			childTransform->addChild(collider);
			collider->childTransform->translate(glm::vec3(cellPos.x, 0.5f, cellPos.z));
			collider->freezeTransformation();
			collider->setColliderAsBoundingBox();
			collider->createRigidBody(0);
			if(sweet::NumberUtils::randomFloat() > 0.75f){
				float s = sweet::NumberUtils::randomInt(1, 2)*0.33f;
				for(auto & v : collider->mesh->vertices){
					v.y *= s;
				}
				cellPos.y *= s;
			}
			cellGrid[collider] = cellPos;


			terrain->mesh->insertVertices(*collider->mesh);
			collider->firstParent()->setVisible(false);
		}
	}



	// CAMERA
	gameCam = new OrthographicCamera(-SIZE, SIZE, -SIZE/2, SIZE/2, -1000, 1000);
	cameras.push_back(gameCam);
	//gameCam->childTransform->addChild(new CameraController(c));
	childTransform->addChild(gameCam);
	activeCamera = gameCam;
	gameCam->yaw = -45;
	gameCam->pitch = -45;
	gameCam->roll = 0;
	

	uiLayer->addMouseIndicator();

	cellHighlight = new MeshEntity(MeshFactory::getCubeMesh(), baseShader);
	cellHighlight->childTransform->scale(glm::vec3(1, 0.1f, 1));
	childTransform->addChild(cellHighlight);


	bulletWorld->update(&sweet::step);
}

MY_Scene_Main::~MY_Scene_Main(){	
	deleteChildTransform();

	diffuseShader->decrementAndDelete();

	delete bulletWorld;
}

void MY_Scene_Main::update(Step * _step){
	glm::uvec2 sd = sweet::getWindowDimensions();
	//gameCam->resize(0, sd.x, 0, sd.y);

	// scene update
	//bulletWorld->update(_step);

	glm::vec3 s = activeCamera->screenToWorld(glm::vec3((float)mouse->mouseX()/sd.x, (float)mouse->mouseY()/sd.y, activeCamera->nearClip), sd);



	//glm::vec3 pos = activeCamera->childTransform->getWorldPos();
	btVector3 start(s.x, s.y, s.z);
	btVector3 dir(activeCamera->forwardVectorRotated.x, activeCamera->forwardVectorRotated.y, activeCamera->forwardVectorRotated.z);
	btVector3 end = start + dir*(activeCamera->farClip - activeCamera->nearClip);
	btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
	bulletWorld->world->rayTest(start, end, rayCallback);

	NodeBulletBody * me;


	// if we're not looking at anything, return nullptr
	if(rayCallback.hasHit()){
		me = static_cast<NodeBulletBody *>(rayCallback.m_collisionObject->getUserPointer());
	}else{
		me = nullptr;
	}

	if(selectedCell != me){
		if(selectedCell != nullptr){
			//selectedCell->setVisible(true);
			cellHighlight->setVisible(false);
		}
		selectedCell = me;
		if(selectedCell != nullptr){
			glm::vec3 cellPos = cellGrid[selectedCell];
			cellHighlight->setVisible(true);
			cellHighlight->firstParent()->translate(cellPos, false);

			//selectedCell->setVisible(false);
		}
	}

	MY_Scene_Base::update(_step);

	// update light position to make it orbit around the scene
	float r = SIZE/2;
	float t = _step->time*0.25f;
	sun->firstParent()->translate(glm::vec3(sin(t) * r, 0, cos(t) * r) + glm::vec3(0, 3, 0), false);
	sun->childTransform->lookAt(glm::vec3(0));

	sun->setIntensities(glm::vec3(glm::min(1.f, glm::sin(t) + 1.f), (glm::sin(t+15)+1)*0.5f, (glm::sin(t*2 + 15)+1)*0.4f+0.1f));
}

void MY_Scene_Main::enableDebug(){
	MY_Scene_Base::enableDebug();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
	childTransform->addChildAtIndex(bulletDebugDrawer, -1, false); // make sure the debug drawer is the last thing drawn
}
void MY_Scene_Main::disableDebug(){
	MY_Scene_Base::disableDebug();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
}