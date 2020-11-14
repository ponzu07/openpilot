#pragma once

#include <QSoundEffect>
#include "sound.hpp"

class QtSound : public Sound {
public:
  QtSound();
<<<<<<< HEAD
  ~QtSound();
=======
>>>>>>> origin/ci-clean
  bool play(AudibleAlert alert);
  void stop();
  void setVolume(int volume);

private:
  std::map<AudibleAlert, QSoundEffect> sounds;
};
