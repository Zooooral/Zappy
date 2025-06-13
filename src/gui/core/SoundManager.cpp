/*
** EPITECH PROJECT, 2025
** src/gui/core/SoundManager.cpp
** File description:
** SoundManager
*/

#include <iostream>

#include "SoundManager.hpp"
#include "ConfigManager.hpp"

SoundManager &SoundManager::getInstance()
{
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager()
{
    if (AUDIO_ENABLED)
        InitAudioDevice();
    if (_musicLoaded) {
        setVolume(ConfigManager::getInstance().getVolume());
    }
}

SoundManager::~SoundManager()
{
    if (AUDIO_ENABLED) {
        if (_musicLoaded) {
            UnloadMusicStream(_music);
        }
        for (auto& soundEffect : _sounds) {
            if (soundEffect.loaded) {
                UnloadSound(soundEffect.sound);
            }
        }
        CloseAudioDevice();
    }
}

void SoundManager::playMusic(const std::string &path, bool loop)
{
    if (!AUDIO_ENABLED)
        return;
    if (_musicLoaded) {
        StopMusicStream(_music);
        UnloadMusicStream(_music);
        _musicLoaded = false;
    }
    _music = LoadMusicStream(path.c_str());
    if (_music.ctxData) {
        PlayMusicStream(_music);
        _musicLoaded = true;
        if (loop) _music.looping = true;
        setVolume(ConfigManager::getInstance().getVolume());
    } else {
        std::cerr << "Failed to load music: " << path << std::endl;
    }
}

void SoundManager::stopMusic()
{
    if (!AUDIO_ENABLED)
        return;
    if (_musicLoaded) {
        StopMusicStream(_music);
        UnloadMusicStream(_music);
        _musicLoaded = false;
    }
}

void SoundManager::setVolume(float volume)
{
    _globalVolume = volume;
    if (AUDIO_ENABLED && _musicLoaded) {
        SetMusicVolume(_music, volume);
    }
    for (auto& soundEffect : _sounds) {
        if (soundEffect.loaded) {
            SetSoundVolume(soundEffect.sound, volume);
        }
    }
}

bool SoundManager::isMusicPlaying() const
{
    if (!AUDIO_ENABLED)
        return false;
    return _musicLoaded && IsMusicStreamPlaying(_music);
}

void SoundManager::updateMusicStream()
{
    if (AUDIO_ENABLED && _musicLoaded) {
        UpdateMusicStream(_music);
    }
}

void SoundManager::updateSounds()
{
    if (!AUDIO_ENABLED)
        return;
    
    for (auto& soundEffect : _sounds) {
        if (soundEffect.loaded && soundEffect.looping && !IsSoundPlaying(soundEffect.sound)) {
            PlaySound(soundEffect.sound);
        }
    }
}

void SoundManager::loadSound(const std::string &name, const std::string &path)
{
    if (!AUDIO_ENABLED)
        return;
    
    SoundEffect* existing = findSound(name);
    if (existing && existing->loaded) {
        UnloadSound(existing->sound);
        existing->loaded = false;
    }
    
    Sound sound = LoadSound(path.c_str());
    if (sound.frameCount > 0) {
        if (!existing) {
            _sounds.push_back({sound, name, true, false});
        } else {
            existing->sound = sound;
            existing->loaded = true;
            existing->looping = false;
        }
        SetSoundVolume(sound, _globalVolume);
    } else {
        std::cerr << "Failed to load sound: " << path << std::endl;
    }
}

void SoundManager::playSound(const std::string &name, bool loop)
{
    if (!AUDIO_ENABLED)
        return;
    
    SoundEffect* soundEffect = findSound(name);
    if (soundEffect && soundEffect->loaded) {
        soundEffect->looping = loop;
        PlaySound(soundEffect->sound);
    }
}

void SoundManager::stopSound(const std::string &name)
{
    if (!AUDIO_ENABLED)
        return;
    
    SoundEffect* soundEffect = findSound(name);
    if (soundEffect && soundEffect->loaded) {
        soundEffect->looping = false;
        StopSound(soundEffect->sound);
    }
}

void SoundManager::stopAllSounds()
{
    if (!AUDIO_ENABLED)
        return;
    
    for (auto& soundEffect : _sounds) {
        if (soundEffect.loaded) {
            soundEffect.looping = false;
            StopSound(soundEffect.sound);
        }
    }
}

void SoundManager::setSoundVolume(const std::string &name, float volume)
{
    if (!AUDIO_ENABLED)
        return;
    
    SoundEffect* soundEffect = findSound(name);
    if (soundEffect && soundEffect->loaded) {
        SetSoundVolume(soundEffect->sound, volume);
    }
}

SoundEffect* SoundManager::findSound(const std::string &name)
{
    for (auto& soundEffect : _sounds) {
        if (soundEffect.name == name) {
            return &soundEffect;
        }
    }
    return nullptr;
}
