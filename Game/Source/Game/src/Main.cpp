#pragma once

#include <Sweet.h>
#include <node\Node.h>
#include <typeinfo>
#include <Texture.h>
#include <Game.h>
#include <MY_ResourceManager.h>
#include <Log.h>
#include <ctime>

#include <MY_Game.h>

#ifdef _DEBUG
	// memory leak debugging
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#ifdef _DEBUG
int main(void){
	_CrtMemState s1;
	_CrtMemCheckpoint( &s1 );
#else
int __stdcall WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show){	
#endif
	Log::THROW_ON_ERROR = true;

	sweet::initialize("Game"); // initialize engine (argument is application title)

	//OpenAL_Sound::masterGain = 0; // mute
#ifdef _DEBUG
	Node::nodeCounting = true; // set this to true if you're checking for memory leaks and stuff (it's really slow so don't do it if you don't need it)
#endif

	// load resources
	MY_ResourceManager * resources = new MY_ResourceManager();

	// create and initialize game
	MY_Game * game = new MY_Game();
	game->init();

	// main game loop
	while (game->isRunning){
		game->performGameLoop();
	}
	
	// deallocate resources
	delete game;
	game = nullptr;
	delete resources;
	resources = nullptr;
	
	sweet::destruct();
	sweet::printNodes();
	
	
#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince(&s1);
#endif
}