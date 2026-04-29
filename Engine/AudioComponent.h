#pragma once

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include "Component.h"

namespace XYZEngine
{
class AudioComponent : public Component
{
  public:
    AudioComponent(GameObject *gameObject);
    ~AudioComponent();

    // from class Component
    void Update(float deltaTime) override;
    void Render() override;

    // Setters
    void SetAudio(const sf::SoundBuffer &audio);
    void SetLoop(bool loop);

    void SetVolume(float volume);
    void Play();
    void Stop();
    void Pause();
    void Resume();

  private:
    sf::Sound *sound;
    // sf::SoundBuffer* soundBuffer;
    bool loop;
};

} // namespace XYZEngine