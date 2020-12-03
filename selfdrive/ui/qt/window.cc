#include <cassert>
#include <iostream>
#include <cmath>
#include <iostream>
#include <fstream>
<<<<<<< HEAD
=======
#include <thread>
>>>>>>> origin/ci-clean
#include <signal.h>

#include <QVBoxLayout>
#include <QMouseEvent>
<<<<<<< HEAD
#include <QPushButton>
#include <QGridLayout>

#include "window.hpp"
#include "settings.hpp"

#include "paint.hpp"
#include "common/util.h"

volatile sig_atomic_t do_exit = 0;

=======

#include "window.hpp"
#include "qt_window.hpp"
#include "offroad/settings.hpp"
#include "offroad/onboarding.hpp"

#include "paint.hpp"
#include "common/util.h"
#include "common/timing.h"

#define BACKLIGHT_DT 0.25
#define BACKLIGHT_TS 2.00

volatile sig_atomic_t do_exit = 0;

static void handle_display_state(UIState *s, int dt, bool user_input) {
  static int awake_timeout = 0;
  awake_timeout = std::max(awake_timeout-dt, 0);

  if (user_input || s->ignition || s->started) {
    s->awake = true;
    awake_timeout = 30*UI_FREQ;
  } else if (awake_timeout == 0){
    s->awake = false;
  }
}

static void set_backlight(int brightness){
  std::ofstream brightness_control("/sys/class/backlight/panel0-backlight/brightness");
  if (brightness_control.is_open()){
    brightness_control << brightness << "\n";
    brightness_control.close();
  }
}

>>>>>>> origin/ci-clean
MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
  main_layout = new QStackedLayout;

#ifdef QCOM2
  set_core_affinity(7);
#endif

<<<<<<< HEAD
<<<<<<< HEAD
  GLWindow * glWindow = new GLWindow(this);
  main_layout->addWidget(glWindow);

  SettingsWindow * settingsWindow = new SettingsWindow(this);
  main_layout->addWidget(settingsWindow);

=======
  GLWindow *glWindow = new GLWindow(this);
=======
  glWindow = new GLWindow(this);
>>>>>>> origin/ci-clean
  main_layout->addWidget(glWindow);

  settingsWindow = new SettingsWindow(this);
  main_layout->addWidget(settingsWindow);

  onboardingWindow = new OnboardingWindow(this);
  main_layout->addWidget(onboardingWindow);
>>>>>>> origin/ci-clean

  main_layout->setMargin(0);
  setLayout(main_layout);
  QObject::connect(glWindow, SIGNAL(openSettings()), this, SLOT(openSettings()));
  QObject::connect(settingsWindow, SIGNAL(closeSettings()), this, SLOT(closeSettings()));

<<<<<<< HEAD
=======
  // start at onboarding
  main_layout->setCurrentWidget(onboardingWindow);
  QObject::connect(onboardingWindow, SIGNAL(onboardingDone()), this, SLOT(closeSettings()));
  onboardingWindow->updateActiveScreen();

>>>>>>> origin/ci-clean
  setStyleSheet(R"(
    * {
      color: white;
      background-color: #072339;
    }
  )");
}

void MainWindow::openSettings() {
  main_layout->setCurrentIndex(1);
  settingsWindow->refreshParams();
}

void MainWindow::closeSettings() {
  main_layout->setCurrentIndex(0);
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event){
  if (event->type() == QEvent::MouseButtonPress) {
    glWindow->wake();
  }
  return false;
}


GLWindow::GLWindow(QWidget *parent) : QOpenGLWidget(parent) {
  timer = new QTimer(this);
  QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));

<<<<<<< HEAD
  int result = read_param(&brightness_b, "BRIGHTNESS_B", true);
  result += read_param(&brightness_m, "BRIGHTNESS_M", true);
  if(result != 0) {
    brightness_b = 0.0;
    brightness_m = 5.0;
=======
  backlight_timer = new QTimer(this);
  QObject::connect(backlight_timer, SIGNAL(timeout()), this, SLOT(backlightUpdate()));

  int result = read_param(&brightness_b, "BRIGHTNESS_B", true);
  result += read_param(&brightness_m, "BRIGHTNESS_M", true);
  if(result != 0) {
    brightness_b = 200.0;
    brightness_m = 10.0;
>>>>>>> origin/ci-clean
  }
  smooth_brightness = 512;
}

GLWindow::~GLWindow() {
  makeCurrent();
  doneCurrent();
}

void GLWindow::initializeGL() {
  initializeOpenGLFunctions();
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  ui_state = new UIState();
  ui_state->sound = &sound;
  ui_state->fb_w = vwp_w;
  ui_state->fb_h = vwp_h;
  ui_init(ui_state);

<<<<<<< HEAD
<<<<<<< HEAD
  timer->start(50);
}

void GLWindow::timerUpdate(){
  // Update brightness
  float clipped_brightness = std::min(1023.0f, (ui_state->light_sensor*brightness_m) + brightness_b);
  smooth_brightness = clipped_brightness * 0.01f + smooth_brightness * 0.99f;

  std::ofstream brightness_control("/sys/class/backlight/panel0-backlight/brightness");
  if (brightness_control.is_open()){
    brightness_control << int(smooth_brightness) << "\n";
    brightness_control.close();
  }

  ui_update(ui_state);

#ifdef QCOM2
  if (ui_state->started != onroad){
    onroad = ui_state->started;
    timer->setInterval(onroad ? 50 : 1000);

    int brightness = onroad ? 1023 : 0;
    std::ofstream brightness_control("/sys/class/backlight/panel0-backlight/brightness");
    if (brightness_control.is_open()){
      brightness_control << int(brightness) << "\n";
      brightness_control.close();
    }
  }
#endif

  update();
=======
=======
  wake();

>>>>>>> origin/ci-clean
  timer->start(0);
  backlight_timer->start(BACKLIGHT_DT * 100);
}

void GLWindow::backlightUpdate(){
  // Update brightness
  float k = (BACKLIGHT_DT / BACKLIGHT_TS) / (1.0f + BACKLIGHT_DT / BACKLIGHT_TS);

  float clipped_brightness = std::min(1023.0f, (ui_state->light_sensor*brightness_m) + brightness_b);
  smooth_brightness = clipped_brightness * k + smooth_brightness * (1.0f - k);
  int brightness = smooth_brightness;

  if (!ui_state->awake){
    brightness = 0;
  }

  std::thread{set_backlight, brightness}.detach();
}

void GLWindow::timerUpdate(){
#ifdef QCOM2
  if (ui_state->started != onroad){
    onroad = ui_state->started;
    timer->setInterval(onroad ? 0 : 1000);
  }
#endif

  // Fix awake timeout if running 1 Hz when offroad
  int dt = timer->interval() == 0 ? 1 : 20;
  handle_display_state(ui_state, dt, false);

  ui_update(ui_state);
  repaint();
>>>>>>> origin/ci-clean
}

void GLWindow::resizeGL(int w, int h) {
  std::cout << "resize " << w << "x" << h << std::endl;
}

void GLWindow::paintGL() {
  ui_draw(ui_state);
}

void GLWindow::wake(){
  // UI state might not be initialized yet
  if (ui_state != nullptr){
    handle_display_state(ui_state, 1, true);
  }
}

void GLWindow::mousePressEvent(QMouseEvent *e) {
  wake();

  // Settings button click
  if (!ui_state->scene.uilayout_sidebarcollapsed && settings_btn.ptInRect(e->x(), e->y())) {
    emit openSettings();
  }

  // Vision click
  if (ui_state->started && (e->x() >= ui_state->scene.viz_rect.x - bdr_s)){
    ui_state->scene.uilayout_sidebarcollapsed = !ui_state->scene.uilayout_sidebarcollapsed;
  }
}


GLuint visionimg_to_gl(const VisionImg *img, EGLImageKHR *pkhr, void **pph) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, *pph);
  glGenerateMipmap(GL_TEXTURE_2D);
  *pkhr = (EGLImageKHR)1; // not NULL
  return texture;
}

void visionimg_destroy_gl(EGLImageKHR khr, void *ph) {
  // empty
}

FramebufferState* framebuffer_init(const char* name, int32_t layer, int alpha,
                                   int *out_w, int *out_h) {
  return (FramebufferState*)1; // not null
}
