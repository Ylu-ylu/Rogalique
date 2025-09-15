#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Engine.h"
#include "ResourceSystem.h" 
#include "DeveloperLevel.h"
#include "Matrix2D.h"
#include <map>
#include <SFML/Audio/SoundBuffer.hpp>
//#include <windows.h>

using namespace XYZRoguelike;

int main()
{
	XYZEngine::RenderSystem::Instance()->SetMainWindow(new sf::RenderWindow(sf::VideoMode(1280, 720), "XYZRoguelike"));

	//XYZEngine::ResourceSystem::Instance()->LoadTexture("ball", "Resources/Textures/ball.png");
	//XYZEngine::ResourceSystem::Instance()->LoadTextureMap("ai", "Resources/TextureMaps/Enemy.png", { 63, 63 }, 5, false);
	//XYZEngine::ResourceSystem::Instance()->LoadTextureMap("Player", "Resources/TextureMaps/Player.png", { 48, 63 }, 4, false);
	XYZEngine::ResourceSystem::Instance()->LoadTextureMap("ai", "Resources/TextureMaps/Wizard.png", {46, 140 }, 1, false);
	XYZEngine::ResourceSystem::Instance()->LoadTextureMap("Player", "Resources/TextureMaps/Man.png", { 45, 140}, 1, false);
	ResourceSystem::Instance()->LoadTextureMap("level_floors", "Resources/TextureMaps/Floor.png", { 16, 16 }, 49, false);
	ResourceSystem::Instance()->LoadTextureMap("level_walls", "Resources/TextureMaps/Wall.png", { 16, 16 }, 48, false);

	XYZEngine::ResourceSystem::Instance()->LoadSound("music", "Resources/Sounds/neon-gaming.wav");
	

	auto developerLevel = std::make_shared<DeveloperLevel>();
	developerLevel->Start();

///For check Matrix2D

	/*XYZEngine::Matrix2D zeroMatrix;
	zeroMatrix.Print();

	XYZEngine::Matrix2D translationMatrix= XYZEngine::Matrix2D(Vector2Df(12.f, 5.f), 0.f, Vector2Df(1.f, 1.f));
	translationMatrix.Print();

	XYZEngine::Matrix2D rotationMatrix = XYZEngine::Matrix2D(Vector2Df(0.f, 0.f), 90.f, Vector2Df(1.f, 1.f));
	rotationMatrix.Print();

	(rotationMatrix*translationMatrix).Print();
	
	XYZEngine::Matrix2D someMatrix = XYZEngine::Matrix2D(Vector2Df(13.f, 25.f), 90.f, Vector2Df(1.5f, 1.f));
	someMatrix.Print();
	
	(someMatrix*someMatrix.GetInversed()).Print();*/	
	
	
	XYZEngine::Engine::Instance()->Run();

	return 0;
}
