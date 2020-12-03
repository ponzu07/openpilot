#include <QUrl>
<<<<<<< HEAD
#include "qt/qt_sound.hpp"
=======
#include "qt_sound.hpp"
>>>>>>> origin/ci-clean

QtSound::QtSound() {
  for (auto &kv : sound_map) {
    auto path = QUrl::fromLocalFile(kv.second.first);
    sounds[kv.first].setSource(path);
  }
}

bool QtSound::play(AudibleAlert alert) {
<<<<<<< HEAD
  sounds[alert].setLoopCount(sound_map[alert].second);
  sounds[alert].setVolume(0.9);
=======
  int loops = sound_map[alert].second> - 1 ? sound_map[alert].second : QSoundEffect::Infinite;
  sounds[alert].setLoopCount(loops);
  sounds[alert].setVolume(0.7);
>>>>>>> origin/ci-clean
  sounds[alert].play();
  return true;
}

void QtSound::stop() {
  for (auto &kv : sounds) {
    // Only stop repeating sounds
    if (sound_map[kv.first].second != 0) {
      kv.second.stop();
    }
  }
}

void QtSound::setVolume(int volume) {
  // TODO: implement this
}
<<<<<<< HEAD

QtSound::~QtSound() {

}
=======
>>>>>>> origin/ci-clean
