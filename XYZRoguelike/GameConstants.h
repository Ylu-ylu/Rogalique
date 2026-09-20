#pragma once

namespace XYZRoguelike
{
    // ==== PLAYER CONSTANTS ====
    constexpr float PLAYER_SPEED = 400.0f;
    constexpr float PLAYER_HEALTH = 100.0f;
    constexpr float PLAYER_ARMOR = 50.0f;
    constexpr float PLAYER_ATTACK = 10.0f;
    constexpr float PLAYER_TEXTURE_HEIGHT = 128.0f;
    constexpr const char *PLAYER_TEXTURE_KEY = "Player";
    constexpr const char *PLAYER_GAMEOBJECT_NAME = "Player";

    // ==== AI CONSTANTS ====
    constexpr float AI_SPEED = 120.0f;
    constexpr float AI_HEALTH = 100.0f;
    constexpr float AI_ARMOR = 0.0f;
    constexpr float AI_ATTACK_POWER = 10.0f;
    constexpr float AI_TEXTURE_HEIGHT = 128.0f;
    constexpr const char *AI_TEXTURE_KEY = "ai";
    constexpr const char *AI_GAMEOBJECT_NAME_PREFIX = "ai";

    // ==== CREEPER CONSTANTS ====
    constexpr float CREEPER_SPEED = 100.0f;
    constexpr float CREEPER_HEALTH = 50.0f;
    constexpr float CREEPER_ARMOR = 10.0f;
    constexpr float CREEPER_ATTACK = 5.0f;
    constexpr int CREEPER_TEXTURE_WIDTH = 80;
    constexpr int CREEPER_TEXTURE_HEIGHT = 80;
    constexpr const char *CREEPER_TEXTURE_KEY = "Creeper";
    constexpr const char *CREEPER_GAMEOBJECT_NAME = "Creeper";

    // ==== CAMERA CONSTANTS ====
    constexpr int CAMERA_WIDTH = 1280;
    constexpr int CAMERA_HEIGHT = 720;

    // ==== GENERAL CONSTANTS ====
    constexpr int DEFAULT_TEXTURE_INDEX = 0;
}
