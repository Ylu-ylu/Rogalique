#include "LevelLoader.h"
#include "GameSettings.h"

#include <fstream>
#include <assert.h>
#include <filesystem>

namespace XYZRoguelike {
	Level& LevelLoader::GetLevel(int i) {
		return levels.at(i);
	}

	


	int LevelLoader::GetLevelCount()
	{
		return levels.size();
	}
}