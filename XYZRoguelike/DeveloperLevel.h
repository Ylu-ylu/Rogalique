#pragma once


#include <iostream>
#include <array>
#include "../Engine/Scene.h"
#include "Player.h"
#include "Music.h"
#include "AI.h"
#include "Floor.h"
#include "Wall.h"

using namespace XYZEngine;

namespace XYZRoguelike
{

	class DeveloperLevel : public Scene
	{
	public:
		void Start() override;
		void Restart() override;
		void Stop() override;

		std::shared_ptr<Player> GetPlayer(); 
	private:
		std::shared_ptr<Player> player;
		std::shared_ptr<AI> ai;
		std::unique_ptr<Music> music;

		std::vector<std::unique_ptr<Wall>> walls;
		std::vector<std::unique_ptr<Floor>> floors;
	};
}