#pragma once

#include <MY_Scene_Main.h>

#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>
#include <shader/ShaderComponentTint.h>

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
#include <sweet/UI.h>
#include <Easing.h>

class RenderSurface;
class StandardFrameBuffer;

MapCell::MapCell(glm::vec3 _position) :
	position(_position),
	unit(nullptr)
{
}

std::map<int, Unit *> MY_Scene_Main::units;

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
	sunTime(0),
	eventManager(new sweet::EventManager()),
	numUnits(0),
	numBaddies(0),
	numFriendlies(0),
	gameOver(false)
{
	// memory management
	screenSurface->incrementReferenceCount();
	screenSurfaceShader->incrementReferenceCount();
	screenFBO->incrementReferenceCount();

	// Setup the debug drawer and add it to the scene
	bulletWorld->world->setDebugDrawer(bulletDebugDrawer);
	childTransform->addChild(bulletDebugDrawer, false);
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);

	ShaderComponentTint * tint;
	diffuseShader->addComponent(new ShaderComponentMVP(diffuseShader));
	diffuseShader->addComponent(new ShaderComponentDiffuse(diffuseShader, false));
	diffuseShader->addComponent(new ShaderComponentTexture(diffuseShader));
	diffuseShader->addComponent(tint = new ShaderComponentTint(diffuseShader));
	diffuseShader->compileShader();
	diffuseShader->name = "Scene: Main, Shader: diffuse";
	diffuseShader->incrementReferenceCount();

	// SUN
	sun = new DirectionalLight(glm::vec3(1), 0.05f);
	childTransform->addChild(sun);
	lights.push_back(sun);

	PointLight * pl = new PointLight(glm::vec3(0.2f), 0, 0.05f, -1);
	sun->firstParent()->addChild(pl, false);
	lights.push_back(pl);

	// add a cube to the light too so that we can see it without having to use the debug mode
	sun->childTransform->addChild(new MeshEntity(MeshFactory::getCubeMesh(0.125), baseShader),false);


	// TERRAIN
	MeshEntity * terrain = new MeshEntity(new TriMesh(true), diffuseShader);

	const TriMesh * const blockMesh = MY_ResourceManager::globalAssets->getMesh("BLOCK")->meshes.at(0);
	const std::vector<TriMesh * > propsMeshes = MY_ResourceManager::globalAssets->getMesh("PROPS")->meshes;

	TriMesh * unitMeshGood = MY_ResourceManager::globalAssets->getMesh("UNIT-GOOD")->meshes.at(0);
	TriMesh * unitMeshBad = MY_ResourceManager::globalAssets->getMesh("UNIT-BAD")->meshes.at(0);
	//unitMeshBad->pushTexture2D(Scenario::defaultTexture->texture); // apply a texture to the baddie mesh to make it easier to debug for now

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

			// lower terrain here
			if(sweet::NumberUtils::randomFloat() > 0.75f){
				float s = sweet::NumberUtils::randomInt(1, 2)*0.33f;
				for(auto & v : collider->mesh->vertices){
					v.y *= s;
				}
				cellPos.y *= s;
			}
			
			// insert prop here
			if(sweet::NumberUtils::randomFloat() > 0.9f){
				MeshInterface * prop = new TriMesh(true);
				prop->insertVertices(*sweet::NumberUtils::randomItem(propsMeshes));
				Transform t;
				t.translate(cellPos)->rotate(sweet::NumberUtils::randomInt(0, 4)/4.f * 360.f, 0, 1, 0, kOBJECT);
				prop->applyTransformation(&t);
				terrain->mesh->insertVertices(*prop);
				delete prop;
			}

			MapCell * cell = new MapCell(cellPos);
			getCellFromPosition(cellPos) = colliderToCell[collider] = cell;

			terrain->mesh->insertVertices(*collider->mesh);
			collider->firstParent()->setVisible(false);


			// spawn unit here
			if(sweet::NumberUtils::randomFloat() > 0.95f){
				bool baddie = sweet::NumberUtils::randomFloat() > 0.75;

				if(baddie){
					++numBaddies;
				}else{
					++numFriendlies;
				}
				++numUnits;

				Unit * u = new Unit(baddie, cellPos, diffuseShader);
				u->id = units.size();
				u->tint = tint;
				units[units.size()] = u;
				cell->unit = u;
				u->cell = cell;
				childTransform->addChild(u);
			}
		}
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

	cellHighlight = new MeshEntity(MeshFactory::getPlaneMesh(), baseShader);
	childTransform->addChild(cellHighlight)->rotate(90, 1, 0, 0, kOBJECT);
	cellHighlight->mesh->setScaleMode(GL_NEAREST);
	cellHighlight->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("HIGHLIGHT")->texture);


	bulletWorld->update(&sweet::step);


	VerticalLinearLayout * vl = new VerticalLinearLayout(uiLayer->world);
	uiLayer->addChild(vl);
	vl->setRationalHeight(1.f, uiLayer);
	vl->setRationalWidth(1.f, uiLayer);
	vl->setMarginBottom(0.05f);
	vl->marginLeft.setRationalSize(1.f, &vl->marginBottom);
	
	TextLabelControlled * lblBaddies = new TextLabelControlled(&numBaddies, 0, numUnits, uiLayer->world, font, textShader);
	vl->addChild(lblBaddies);
	lblBaddies->prefix = "Sleeper Agents: ";
	lblBaddies->suffix = " / " + std::to_string((int)numBaddies);
	TextLabelControlled * lblFriendlies = new TextLabelControlled(&numFriendlies, 0, numUnits, uiLayer->world, font, textShader);
	vl->addChild(lblFriendlies);
	lblFriendlies->prefix = "Civilians: ";
	lblFriendlies->suffix = " / " + std::to_string((int)numFriendlies);

	eventManager->addEventListener("kill", [this](sweet::Event * _event){
		kill(_event->getIntData("unit"));
	});


	fade = new NodeUI(uiLayer->world);
	fade->setRationalHeight(1.f, uiLayer);
	fade->setRationalWidth(1.f, uiLayer);
	uiLayer->addChild(fade);
	fade->setBackgroundColour(0,0,0, 1);

	Timeout * fadeInTimer = new Timeout(1.f, [this](sweet::Event * _event){
		fade->setVisible(false);
	});
	fadeInTimer->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		fade->setBackgroundColour(0,0,0, Easing::easeInOutCubic(_event->getFloatData("progress"), 1, -1, 1));
	});
	fadeInTimer->start();
	childTransform->addChild(fadeInTimer);

	fadeOutTimer = new Timeout(1.f, [this](sweet::Event * _event){
		fade->setBackgroundColour(0,0,0, 0.5f);
	});
	fadeOutTimer->eventManager->addEventListener("start", [this](sweet::Event * _event){
		fade->setBackgroundColour(0,0,0, 0);
		fade->setVisible(true);
	});
	fadeOutTimer->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		fade->setBackgroundColour(0,0,0, Easing::easeInOutCubic(_event->getFloatData("progress"), 0, 0.5f, 1));
	});
	childTransform->addChild(fadeOutTimer);
}

MY_Scene_Main::~MY_Scene_Main(){	
	deleteChildTransform();

	diffuseShader->decrementAndDelete();

	delete bulletWorld;

	for(auto m : colliderToCell){
		delete m.second;
	}
	
	// memory management
	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();

	delete eventManager;

	MY_ResourceManager::globalAssets->getAudio("DEATH")->sound->parents.clear();
	units.clear();
}

void MY_Scene_Main::update(Step * _step){
	OpenAL_Sound::setListener(activeCamera);


	// check for win/lose
	if(!gameOver){
		if(numBaddies == 0){
			TextLabel * win = new TextLabel(uiLayer->world, MY_ResourceManager::globalAssets->getFont("FONT-BIG")->font, textShader);
			uiLayer->addChild(win);
			win->setRationalHeight(1.f, uiLayer);
			win->setRationalWidth(1.f, uiLayer);
			win->horizontalAlignment = kCENTER;
			win->verticalAlignment = kMIDDLE;
			win->setText("YOU WIN");
			gameOver = true;
			fadeOutTimer->start();
		}
		if(numFriendlies == 0){
			TextLabel * lose = new TextLabel(uiLayer->world, MY_ResourceManager::globalAssets->getFont("FONT-BIG")->font, textShader);
			uiLayer->addChild(lose);
			lose->setRationalHeight(1.f, uiLayer);
			lose->setRationalWidth(1.f, uiLayer);
			lose->horizontalAlignment = kCENTER;
			lose->verticalAlignment = kMIDDLE;
			lose->setText("YOU LOSE");
			gameOver = true;
			fadeOutTimer->start();
		}
	}



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
			cellHighlight->firstParent()->translate(colliderToCell[selectedCell]->position + glm::vec3(0, 0.1, 0), false);

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
				selectedUnit->targetPosition = selectedUnit->currentPosition;
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
	for(auto it : units){
		Unit * u = it.second;
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
						// path blocked

						if(
							u != targetCell->unit // don't kill yourself
							&& (u->team == 1 || u == selectedUnit)// you're a baddie OR you're the selected unit
							&& targetCell->unit != selectedUnit // selected unit is invincible
						){
							// just straight murder em
							sweet::Event * e = new sweet::Event("kill");
							e->setIntData("unit", targetCell->unit->id);
							eventManager->triggerEvent(e);
							u->killTimeout->restart();
							continue;
						}else{
							//try the other route
							if(glm::abs(d.x) > glm::abs(d.z)){
								movement.z += glm::sign(d.z);
								movement.x -= glm::sign(d.x);
							}else{
								movement.x += glm::sign(d.x);
								movement.z -= glm::sign(d.z);
							}
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

	eventManager->update(_step);
	MY_Scene_Base::update(_step);

	// update light position to make it orbit around the scene
	float r = SIZE/2;
	sunTime += _step->deltaTime*0.25f;
	sun->firstParent()->translate(glm::vec3(sin(sunTime) * r, 0, cos(sunTime) * r) + glm::vec3(0, 4, 0), false);
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
	// unbind our screen framebuffer, rebinding the previously bound framebuffer
	// since we didn't have one bound before, this will be the default framebuffer (i.e. the one visible to the player)
	FrameBufferInterface::popFbo();

	// render our screen framebuffer using the standard render surface
	screenSurface->render(screenFBO->getTextureId());

	// render the uiLayer after the screen surface in order to avoid hiding it through shader code
	uiLayer->setVisible(true);
	uiLayer->render(_matrixStack, _renderOptions);
}

MapCell *& MY_Scene_Main::getCellFromPosition(glm::vec3 _position){
	return positionToCell[std::make_pair(glm::floor(_position.x), glm::floor(_position.z))];
}


void MY_Scene_Main::kill(Unit * _unit){
	for(auto u : units){
		if(u.second == _unit){
			kill(u.first);
			break;
		}
	}
}
	
void MY_Scene_Main::kill(int _unitId){
	auto it = units.find(_unitId);
	if(it != units.end()){

		Unit * u = it->second;
		units.erase(_unitId);
		u->cell->unit = nullptr;
		if(u == selectedUnit){
			selectedUnit = nullptr;
		}

		if(u->team == 1){
			numBaddies -= 1;
		}else{
			numFriendlies -= 1;
		}

		Timeout * deathTimer = new Timeout(1.f, [u](sweet::Event * _event){
			u->firstParent()->firstParent()->removeChild(u->firstParent());
			delete u->firstParent();
		});
		
		float y = u->currentPosition.y;
		deathTimer->eventManager->addEventListener("progress", [this, u, y](sweet::Event * _event){
			float p = Easing::easeOutCubic(_event->getFloatData("progress"), y, 1.5f, 1);
			u->targetPosition.y = u->currentPosition.y = p;
		});

		childTransform->addChild(deathTimer)->translate(u->childTransform->getWorldPos());
		deathTimer->start();

		
		OpenAL_Sound * deathSound = MY_ResourceManager::globalAssets->getAudio("DEATH")->sound;
		if(deathSound->parents.size() != 0){
			deathSound->firstParent()->removeChild(deathSound);
		}
		deathTimer->firstParent()->addChild(deathSound, false);
		deathSound->play(false);
	}
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

glm::vec3 MY_Scene_Main::getRandomUnitPosition(){
	Unit * u;
	do{
		int i = sweet::NumberUtils::randomInt(0, units.size()-1);
		auto t = units.begin();
		std::advance( t, i );
		u = t->second;
	}while(u->team == 1);

	return u->currentPosition;
}