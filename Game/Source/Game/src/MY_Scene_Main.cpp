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
#include <Unit.h>
#include <RenderOptions.h>
#include <RenderSurface.h>
#include <StandardFrameBuffer.h>

class RenderSurface;
class StandardFrameBuffer;

MapCell::MapCell(glm::vec3 _position) :
	position(_position),
	unit(nullptr)
{
}

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	diffuseShader(new ComponentShaderBase(true)),
	bulletWorld(new BulletWorld()),
	bulletDebugDrawer(new BulletDebugDrawer(bulletWorld->world)),
	selectedCell(nullptr),
	selectedUnit(nullptr),
	camAngle(0),
	screenSurfaceShader(new Shader("assets/RenderSurface_1", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader, true)),
	screenFBO(new StandardFrameBuffer(true)),
	sunTime(0)
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
	MeshEntity * terrain = new MeshEntity(new TriMesh(true), diffuseShader);

	const TriMesh * const blockMesh = MY_ResourceManager::globalAssets->getMesh("BLOCK")->meshes.at(0);
	const std::vector<TriMesh * > propsMeshes = MY_ResourceManager::globalAssets->getMesh("PROPS")->meshes;

	childTransform->addChild(terrain);
	for(signed long int x = 0; x < SIZE; ++x){
		for(signed long int y = 0; y < SIZE; ++y){
			glm::vec3 cellPos(x - SIZE/2, 1, y - SIZE/2);
			//MeshEntity * me = new MeshEntity(cubeMesh, diffuseShader);
			//terrain->addChild(me)->translate(x, 0, y)->scale(glm::vec3(1, sweet::NumberUtils::randomInt(1, 3)*0.33f, 1));

			BulletMeshEntity * collider = new BulletMeshEntity(bulletWorld, new TriMesh(true), diffuseShader);
			collider->mesh->insertVertices(*blockMesh);

			childTransform->addChild(collider);
			collider->childTransform->translate(glm::vec3(cellPos.x, 0.f, cellPos.z));
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
			
			if(sweet::NumberUtils::randomFloat() > 0.9f){
				MeshInterface * prop = new TriMesh(true);
				prop->insertVertices(*sweet::NumberUtils::randomItem(propsMeshes));
				Transform t;
				t.translate(cellPos)->rotate(sweet::NumberUtils::randomInt(0, 4)/4.f * 360.f, 0, 1, 0, kOBJECT);
				prop->applyTransformation(&t);
				terrain->mesh->insertVertices(*prop);
				delete prop;
			}

			colliderToCell[collider] = new MapCell(cellPos);
			getCellFromPosition(cellPos) = colliderToCell[collider];

			terrain->mesh->insertVertices(*collider->mesh);
			collider->firstParent()->setVisible(false);
		}
	}

	
	units.push_back(new Unit(0, glm::vec3(1, 1, 3), diffuseShader));
	units.push_back(new Unit(0, glm::vec3(2, 1, 3), diffuseShader));
	units.push_back(new Unit(0, glm::vec3(3, 1, 4), diffuseShader));
	for(Unit * u : units){
		getCellFromPosition(u->currentPosition)->unit = u;
		u->cell = getCellFromPosition(u->currentPosition);
		childTransform->addChild(u);
	}

	// CAMERA
	gameCam = new OrthographicCamera(-SIZE, SIZE, -SIZE/2, SIZE/2, -1000, 1000);
	cameras.push_back(gameCam);
	//gameCam->childTransform->addChild(new CameraController(c));
	childTransform->addChild(gameCam)->translate(glm::vec3(-0.5,0,-0.5));
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

	for(auto m : colliderToCell){
		delete m.second;
	}
}

void MY_Scene_Main::update(Step * _step){
	// camera controls
	if(keyboard->keyJustDown(GLFW_KEY_A) || keyboard->keyJustDown(GLFW_KEY_LEFT) || mouse->getMouseWheelDelta() < -FLT_EPSILON){
		--camAngle;
		if(camAngle < 0){
			camAngle = 3;
			gameCam->yaw += 360.f;
		}
	}if(keyboard->keyJustDown(GLFW_KEY_D) || keyboard->keyJustDown(GLFW_KEY_RIGHT) || mouse->getMouseWheelDelta() > FLT_EPSILON){
		++camAngle;
		if(camAngle > 3){
			camAngle = 0;
			gameCam->yaw -= 360.f;
		}
	}
	gameCam->yaw += ((camAngle/4.f*360.f + 45.f) - gameCam->yaw) * 0.1f;



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
			cellHighlight->setVisible(true);
			cellHighlight->firstParent()->translate(colliderToCell[selectedCell]->position, false);

			//selectedCell->setVisible(false);
		}
	}


	if(selectedCell != nullptr){
		if(mouse->leftJustPressed()){
			MapCell * cell = colliderToCell[selectedCell];

			// de-select current unit
			if(selectedUnit != nullptr){
				selectedUnit->setShader(diffuseShader, false);
				selectedUnit->waitTimeout->restart();
			}
			
			// select the unit in the cell
			selectedUnit = cell->unit;
			if(selectedUnit != nullptr){
				selectedUnit->setShader(baseShader, false);
				selectedUnit->waitTimeout->stop();
				selectedUnit->wanderTimeout->stop();
			}
		}else if(mouse->rightJustPressed()){

			// move unit
			MapCell * cell = colliderToCell[selectedCell];
			
			if(selectedUnit != nullptr){
				selectedUnit->targetPosition = cell->position;
			}
		}
	}



	// move units
	for(Unit * u : units){
		if(u->canMove){
			glm::vec3 d = u->targetPosition - (u->currentPosition - glm::vec3(glm::mod(u->currentPosition,glm::vec3(1.f))));
			if(glm::length(d) > FLT_EPSILON){
				glm::vec3 movement(0);
				if(glm::abs(d.x) > glm::abs(d.z)){
					movement.x += glm::sign(d.x);
				}else{
					movement.z += glm::sign(d.z);
				}
			
				unsigned long int count = 0;
				do{
					glm::vec3 newPos = u->currentPosition + movement/* * 0.05f*/;
					MapCell * targetCell = getCellFromPosition(newPos);

					if(targetCell->unit == nullptr || targetCell->unit == u){
						u->cell->unit = nullptr;
						u->currentPosition = newPos;
						u->currentPosition.y = targetCell->position.y;
						targetCell->unit = u;
						u->cell = targetCell;
						u->moveTimeout->restart();
						u->canMove = false;
						break;
					}else{
						// path blocked, try the other route
						if(glm::abs(d.x) > glm::abs(d.z)){
							movement.z += glm::sign(d.z);
							movement.x -= glm::sign(d.x);
						}else{
							movement.x += glm::sign(d.x);
							movement.z -= glm::sign(d.z);
						}
						continue;
					}
				}while(++count <= 1);
				
				if(movement.x == 0 && movement.z == 0){
					// path fully blocked
				}
			}
		}
	}

	MY_Scene_Base::update(_step);

	// update light position to make it orbit around the scene
	float r = SIZE/2;
	sunTime += _step->deltaTime*0.25f;
	sun->firstParent()->translate(glm::vec3(sin(sunTime) * r, 0, cos(sunTime) * r) + glm::vec3(0, 3, 0), false);
	sun->childTransform->lookAt(glm::vec3(0));

	sun->setIntensities(glm::vec3(glm::min(1.f, glm::sin(sunTime) + 1.f), (glm::sin(sunTime+15)+1)*0.5f, (glm::sin(sunTime*2 + 15)+1)*0.4f+0.1f));





	
	// Screen shader update
	// Screen shaders are typically loaded from a file instead of built using components, so to update their uniforms
	// we need to use the OpenGL API calls

	if(keyboard->keyJustDown(GLFW_KEY_L)){
		screenSurfaceShader->unload();
		screenSurfaceShader->loadFromFile(screenSurfaceShader->vertSource, screenSurfaceShader->fragSource);
		screenSurfaceShader->load();
	}

	screenSurfaceShader->bindShader(); // remember that we have to bind the shader before it can be updated
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "time");
	checkForGlError(0);
	if(test != -1){
		glUniform1f(test, _step->time);
		checkForGlError(0);
	}
}

void MY_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	glm::vec3 v =sun->getIntensities() * 0.5f;
	_renderOptions->setClearColour(v.x, v.y, v.z, 1);

	// keep our screen framebuffer up-to-date with the current viewport
	screenFBO->resize(_renderOptions->viewPortDimensions.width, _renderOptions->viewPortDimensions.height);

	// bind our screen framebuffer
	FrameBufferInterface::pushFbo(screenFBO);
	// render the scene
	uiLayer->setVisible(false);
	MY_Scene_Base::render(_matrixStack, _renderOptions);
	uiLayer->setVisible(true);
	// unbind our screen framebuffer, rebinding the previously bound framebuffer
	// since we didn't have one bound before, this will be the default framebuffer (i.e. the one visible to the player)
	FrameBufferInterface::popFbo();

	// render our screen framebuffer using the standard render surface
	screenSurface->render(screenFBO->getTextureId());

	// render the uiLayer after the screen surface in order to avoid hiding it through shader code
	uiLayer->render(_matrixStack, _renderOptions);
}

MapCell *& MY_Scene_Main::getCellFromPosition(glm::vec3 _position){
	return positionToCell[std::make_pair(glm::floor(_position.x), glm::floor(_position.z))];
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