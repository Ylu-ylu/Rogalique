#include "pch.h"
#include "SpriteMovementAnimationComponent.h"
#include "ResourceSystem.h"

namespace XYZEngine
{
SpriteMovementAnimationComponent::SpriteMovementAnimationComponent(GameObject *gameObject) : Component(gameObject)
{
    spriteRenderer = gameObject->GetComponent<SpriteRendererComponent>();
}

void SpriteMovementAnimationComponent::Update(float deltaTime)
{
    if (spriteRenderer == nullptr || !isPlaying)
    {
        return;
    }

    auto animationIt = animations.find(currentAnimationName);
    if (animationIt == animations.end())
    {
        isPlaying = false;
        return;
    }

    AnimationClip &clip = animationIt->second;

    if (clip.frames.empty())
    {
        isPlaying = false;
        return;
    }

    currentTime += deltaTime;
    if (currentTime < clip.frameTime)
    {
        return;
    }

    currentTime = 0.f;
    currentFrameIndex++;

    if (currentFrameIndex >= static_cast<int>(clip.frames.size()))
    {
        if (clip.loop)
        {
            currentFrameIndex = 0;
        }
        else
        {
            currentFrameIndex = static_cast<int>(clip.frames.size()) - 1;
            isPlaying = false;
        }
    }

    const sf::Texture *frameTexture =
        ResourceSystem::Instance()->GetTextureMapElementShared(clip.textureMapName, clip.frames[currentFrameIndex]);
    if (frameTexture != nullptr)
    {
        spriteRenderer->SetTexture(*frameTexture);
    }
}

void SpriteMovementAnimationComponent::Render()
{
}

void SpriteMovementAnimationComponent::AddAnimation(const std::string &name, const std::string &textureMapName,
                                                    const std::vector<int> &frames, float frameTime, bool loop)
{
    if (frames.empty())
    {
        return;
    }

    if (frameTime <= 0.f)
    {
        frameTime = 0.1f;
    }

    AnimationClip clip;
    clip.textureMapName = textureMapName;
    clip.frames = frames;
    clip.frameTime = frameTime;
    clip.loop = loop;
    animations[name] = clip;
}

void SpriteMovementAnimationComponent::Play(const std::string &name)
{
    if (currentAnimationName == name && isPlaying)
    {
        return;
    }

    auto animationIt = animations.find(name);
    if (animationIt == animations.end())
    {
        return;
    }

    const AnimationClip &clip = animationIt->second;
    if (clip.frames.empty())
    {
        return;
    }

    currentAnimationName = name;
    currentFrameIndex = 0;
    currentTime = 0.f;
    isPlaying = true;

    const sf::Texture *frameTexture =
        ResourceSystem::Instance()->GetTextureMapElementShared(clip.textureMapName, clip.frames[currentFrameIndex]);
    if (frameTexture != nullptr)
    {
        spriteRenderer->SetTexture(*frameTexture);
    }
}

void SpriteMovementAnimationComponent::Stop()
{
    isPlaying = false;
}

bool SpriteMovementAnimationComponent::IsPlaying(const std::string &name) const
{
    return isPlaying && currentAnimationName == name;
}

bool SpriteMovementAnimationComponent::IsCurrentAnimation(const std::string &name) const
{
    return currentAnimationName == name;
}
} // namespace XYZEngine
