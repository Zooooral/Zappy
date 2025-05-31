/*
** EPITECH PROJECT, 2025
** src/gui/core/SoundManager.hpp
** File description:
** SoundManager
*/

#ifndef SOUNDMANAGER_HPP_
    #define SOUNDMANAGER_HPP_

#include "raylib.h"

#include <string>
#include <vector>

struct SoundEffect {
    Sound sound;
    std::string name;
    bool loaded;
    bool looping;
};

class SoundManager {
public:
    static constexpr bool AUDIO_ENABLED = true;
    static SoundManager &getInstance();
    void playMusic(const std::string &path, bool loop = true);
    void stopMusic();
    void setVolume(float volume);
    bool isMusicPlaying() const;
    void updateMusicStream();
    void updateSounds();
    
    void loadSound(const std::string &name, const std::string &path);
    void playSound(const std::string &name, bool loop = false);
    void stopSound(const std::string &name);
    void stopAllSounds();
    void setSoundVolume(const std::string &name, float volume);

private:
    SoundManager();
    ~SoundManager();
    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;

    Music _music;
    bool _musicLoaded = false;
    std::vector<SoundEffect> _sounds;
    float _globalVolume = 1.0f;
    
    SoundEffect* findSound(const std::string &name);
};

#endif /* !SOUNDMANAGER_HPP_ */
