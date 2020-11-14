#pragma once

#include <QWidget>
#include <QTimer>
<<<<<<< HEAD
#include <QLabel>
#include <QGuiApplication>
=======
>>>>>>> origin/ci-clean
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QStackedLayout>

#include "qt/qt_sound.hpp"
#include "ui/ui.hpp"

<<<<<<< HEAD
class MainWindow : public QWidget
{
=======
class MainWindow : public QWidget {
>>>>>>> origin/ci-clean
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);

private:
  QStackedLayout *main_layout;

public slots:
  void openSettings();
  void closeSettings();
<<<<<<< HEAD

=======
>>>>>>> origin/ci-clean
};

#ifdef QCOM2
const int vwp_w = 2160;
#else
const int vwp_w = 1920;
#endif
const int vwp_h = 1080;
<<<<<<< HEAD
class GLWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
=======

class GLWindow : public QOpenGLWidget, protected QOpenGLFunctions {
>>>>>>> origin/ci-clean
  Q_OBJECT

public:
  using QOpenGLWidget::QOpenGLWidget;
  explicit GLWindow(QWidget *parent = 0);
  ~GLWindow();

protected:
  void mousePressEvent(QMouseEvent *e) override;
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

<<<<<<< HEAD

private:
  QTimer * timer;
=======
private:
  QTimer * timer;
  QTimer * backlight_timer;

>>>>>>> origin/ci-clean
  UIState * ui_state;
  QtSound sound;

  bool onroad = true;
<<<<<<< HEAD
  QLabel * label = NULL;
=======

  // TODO: this shouldn't be here
>>>>>>> origin/ci-clean
  float brightness_b = 0;
  float brightness_m = 0;
  float smooth_brightness = 0;

public slots:
  void timerUpdate();
<<<<<<< HEAD
=======
  void backlightUpdate();
>>>>>>> origin/ci-clean

signals:
  void openSettings();
};
