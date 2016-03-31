#pragma once

#include <MY_Scene_Main.h>

#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>

#include <MeshFactory.h>

#include <DirectionalLight.h>
#include <NumberUtils.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
	diffuseShader(new ComponentShaderBase(true))
{
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

	// add a cube to the light too so that we can see it without having to use the debug mode
	sun->childTransform->addChild(new MeshEntity(MeshFactory::getCubeMesh(), baseShader),false);


	// TERRAIN
	QuadMesh * cubeMesh = MeshFactory::getCubeMesh();
	Transform * terrain = new Transform();
	terrain->translate(0, 0.5f, 0, false);
	cubeMesh->applyTransformation(terrain);
		
	childTransform->addChild(terrain, false);
	terrain->translate(-8, 0, -8, false);
	for(unsigned long int x = 0; x < 16; ++x){
	for(unsigned long int y = 0; y < 16; ++y){
		MeshEntity * me = new MeshEntity(cubeMesh, diffuseShader);
		terrain->addChild(me)->translate(x, 0, y)->scale(glm::vec3(1, sweet::NumberUtils::randomInt(1, 3)*0.33f, 1));
	}
	}



	// CAMERA
	gameCam = new OrthographicCamera(-16, 16, -8, 8, -1000, 1000);
	cameras.push_back(gameCam);
	//gameCam->childTransform->addChild(new CameraController(c));
	childTransform->addChild(gameCam);
	activeCamera = gameCam;
	gameCam->yaw = -45;
	gameCam->pitch = -45;
	gameCam->roll = 0;
}

MY_Scene_Main::~MY_Scene_Main(){	
	diffuseShader->decrementAndDelete();
}

void MY_Scene_Main::update(Step * _step){
	//glm::uvec2 sd = sweet::getWindowDimensions();
	//gameCam->resize(0, sd.x, 0, sd.y);

	// scene update
	MY_Scene_Base::update(_step);

	// update light position to make it orbit around the scene
	float r = 8;
	float t = _step->time*0.25f;
	sun->firstParent()->translate(glm::vec3(sin(t) * r, 0, cos(t) * r) + glm::vec3(0, 3, 0), false);
	sun->childTransform->lookAt(glm::vec3(0));

	sun->setIntensities(glm::vec3(glm::min(1.f, glm::sin(t) + 1.f), (glm::sin(t+15)+1)*0.5f, (glm::sin(t*2 + 15)+1)*0.4f+0.1f));
}