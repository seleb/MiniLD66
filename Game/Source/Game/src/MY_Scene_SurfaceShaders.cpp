#pragma once

#include <MY_Scene_SurfaceShaders.h>

#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>
#include <shader/ShaderComponentPhong.h>
#include <shader/ShaderComponentBlinn.h>
#include <shader/ShaderComponentToon.h>
#include <RampTexture.h>

#include <MeshFactory.h>

#include <PointLight.h>
#include <Material.h>

#include <CubeMap.h>

MY_Scene_SurfaceShaders::MY_Scene_SurfaceShaders(Game * _game) :
	MY_Scene_Base(_game),
	diffuseShader(new ComponentShaderBase(true)),
	phongShader(new ComponentShaderBase(true)),
	blinnShader(new ComponentShaderBase(true)),
	toonShader(new ComponentShaderBase(true))
{
	diffuseShader->addComponent(new ShaderComponentMVP(diffuseShader));
	diffuseShader->addComponent(new ShaderComponentDiffuse(diffuseShader, false));
	diffuseShader->addComponent(new ShaderComponentTexture(diffuseShader));
	diffuseShader->compileShader();
	diffuseShader->name = "Scene: Surface Shaders, Shader: diffuse";

	phongShader->addComponent(new ShaderComponentMVP(phongShader));
	phongShader->addComponent(new ShaderComponentPhong(phongShader));
	phongShader->addComponent(new ShaderComponentTexture(phongShader));
	phongShader->compileShader();
	phongShader->name = "Scene: Surface Shaders, Shader: phong";

	blinnShader->addComponent(new ShaderComponentMVP(blinnShader));
	blinnShader->addComponent(new ShaderComponentBlinn(blinnShader));
	blinnShader->addComponent(new ShaderComponentTexture(blinnShader));
	blinnShader->compileShader();
	blinnShader->name = "Scene: Surface Shaders, Shader: blinn";

	toonShader->addComponent(new ShaderComponentMVP(toonShader));
	toonShader->addComponent(new ShaderComponentToon(toonShader, new RampTexture(glm::vec3(0), glm::vec3(1), 5), false));
	toonShader->addComponent(new ShaderComponentTexture(toonShader));
	toonShader->compileShader();
	toonShader->name = "Scene: Surface Shaders, Shader: toon";

	// add all of our surface shaders to a list so that we can easily assign them in a loop
	shaders.push_back(baseShader);
	shaders.push_back(diffuseShader);
	shaders.push_back(phongShader);
	shaders.push_back(blinnShader);
	shaders.push_back(toonShader);

	// Create a material. Phong and Bl
	Material * material = new Material(80, glm::vec3(1,0,0), true);

	
	// add a bunch of cubes arranged in a circle
	MeshInterface * cubeMesh = MeshFactory::getCubeMesh();
	cubeMesh->pushMaterial(material);
	for(signed long int i = 0; i < 32; ++i){
		MeshEntity * me = new MeshEntity(cubeMesh, shaders.at(i % shaders.size()));
		childTransform->addChild(me)->translate(cos(i/32.f * glm::pi<float>()*2)*10, 0, sin(i/32.f * glm::pi<float>()*2)*10, false);
		//me->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
	}

	// add a bunch of planes as a ground
	MeshInterface * planeMesh = MeshFactory::getPlaneMesh(0.5);
	planeMesh->pushMaterial(material);
	for(signed long int x = 0; x < 32; ++x){
	for(signed long int y = 0; y < 32; ++y){
		MeshEntity * me = new MeshEntity(planeMesh, shaders.at((x+y*32) % shaders.size()));
		childTransform->addChild(me)->translate((x-16), 0, (y-16), false)->rotate(-90, 1, 0, 0, kOBJECT);
		//me->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
	}
	}

	// create our light and add it to the scene
	light = new PointLight(glm::vec3(1), 0.05f, 0.001f, -1);
	childTransform->addChild(light);
	lights.push_back(light);

	// add a cube to the light too so that we can see it without having to use the debug mode
	light->childTransform->addChild(new MeshEntity(cubeMesh, baseShader),false);

	// add a cubemap (cubemaps use a special texture type and shader component. these can be instantiated separately if desired, but the CubeMap class handles them both for us)
	CubeMap * cubemap = new CubeMap("assets/textures/cubemap", "png");
	childTransform->addChild(cubemap);

	for(auto s : shaders){
		s->incrementReferenceCount();
	}
}

MY_Scene_SurfaceShaders::~MY_Scene_SurfaceShaders(){	
	for(auto s : shaders){
		s->decrementAndDelete();
	}
}

void MY_Scene_SurfaceShaders::update(Step * _step){
	// scene update
	MY_Scene_Base::update(_step);

	// update light position to make it orbit around the scene
	float r = /*sin(_step->time/8)**/16;
	light->firstParent()->translate(sin(_step->time) * r, 3, cos(_step->time) * r, false);
}