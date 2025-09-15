#include "Music.h"

Music::Music(const std::string& soundName)
{
	gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject("Music: " + soundName);
	auto music = gameObject->AddComponent<XYZEngine::AudioComponent>();
    const sf::SoundBuffer* buffer = XYZEngine::ResourceSystem::Instance()->GetSound(soundName);
    if (buffer) {
        music->SetAudio(*buffer);
        music->SetLoop(true);
        music->Play();
    }
    else {
        // Handle error: log, throw, or fallback
        std::cerr << "Failed to load sound: " << soundName << std::endl;
    }
	

}
