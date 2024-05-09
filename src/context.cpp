
#include <chrono>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <typeinfo>
#include <vector>

#include "context.h"
#include "entity.h"
#include "settings.h"

using namespace steelcpp;

Context::Context(Vector2 screenSize, const char *windowTitle) {
  screenSize_.x = screenSize.x;
  screenSize_.y = screenSize.y;
  windowTitle_ = std::string(windowTitle);
  setup();
}

Context::~Context() { CloseWindow(); }

Vector2 Context::getScreenSize() const { return screenSize_; }

void Context::calculateDeltaFrameTime() {
  auto currentFrameTime = std::chrono::steady_clock::now();
  deltaFrameTime_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        currentFrameTime - lastFrameTime_)
                        .count();

  if (deltaFrameTime_ < 0) {
    fprintf(stdout, "Unexpected negative delta_time: %ld\n", deltaFrameTime_);
    // Consider handling this case more robustly, e.g., by resetting
    // last_frame_time
  }

  lastFrameTime_ = currentFrameTime;
}

long Context::getDeltaFrameTime() const {
  /*
    Return the elapsed time in nanoseconds since the last frame
  */
  return deltaFrameTime_;
}

Player &Context::getPlayer() { return *player1_; }

int Context::setup() {
  isRunning = true;
  state = START_SCREEN;
  lastFrameTime_ = std::chrono::steady_clock::now();

  InitWindow(screenSize_.x, screenSize_.y, windowTitle_.c_str());
  // Player(Context, Name, Size, Position, Speed, HP, Color)
  player1_ = new Player(this, std::string("Player 1"), {20, 20}, {100, 100},
                        {150, 150}, 5, RED);
  player1_->setInvincibleTime(3.0f);
  entities.push_back(static_cast<Entity *>(player1_));
  // Fruit(Context, Name, Size, Posiiton, ShootReloadTime(ms), Color)
  Fruit *fruit = new Fruit(this, std::string("Fruit1"), {10, 10},
                           {(float)GetRandomValue(0, screenSize_.x - 10),
                            (float)GetRandomValue(0, screenSize_.y - 10)},
                           BLUE, 1.5f);
  fruits.push_back(fruit);
  entities.push_back(static_cast<Entity *>(fruit));
  fruit = new Fruit(this, std::string("Fruit2"), {10, 10},
                    {(float)GetRandomValue(0, screenSize_.x - 10),
                     (float)GetRandomValue(0, screenSize_.y - 10)},
                    BLUE, 1.5f);
  fruits.push_back(fruit);
  entities.push_back(static_cast<Entity *>(fruit));
  fprintf(stdout, "Entities %zu\n", entities.size());
  return 0;
}

void Context::update() {
  if (!WindowShouldClose()) {
    processInput();
    if (state == GAME_SCREEN)
      updateEntities();
    render();
  } else
    isRunning = false;
}

void Context::processInput() {
  player1_->handleKeyPressed();
  player1_->handleKeyReleased();
  if (state != END_SCREEN) {
  }

  if (IsKeyPressed(KEY_P) && (state == END_SCREEN || state == START_SCREEN)) {
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    player1_->setHealthPoints(5);
    player1_->setScore(0);
    for (auto fruit : fruits) {
      fruit->setPosition(
          {(float)GetRandomValue(0, screenSize_.x - fruit->getSize().x),
           (float)GetRandomValue(0, screenSize_.y - fruit->getSize().y)});
    }
    state = GAME_SCREEN;
  }

  if (IsKeyDown(KEY_ESCAPE)) {
    isRunning = false;
    CloseWindow();
  }
}

void Context::updateEntities() {

  calculateDeltaFrameTime();
  std::string e_n;
  // std::erase_if(entities,  [](Entity* entity){return
  // entity->shouldDestroy;});
  int i = 0;
  struct timespec ct;
  clock_gettime(CLOCK_MONOTONIC, &ct);
  float dt = ((float)ct.tv_sec + (float)ct.tv_nsec / BILLION) -
             ((float)startTime.tv_sec + (float)startTime.tv_nsec / BILLION);
  for (auto entity : entities) {

    const std::type_info &e_t = typeid(*entity);
    entity->update();

    if (entity->shouldDestroy) {
      entities.erase(entities.begin() + i);
      delete entity;
      i++;
      continue;
    }

    e_n = std::string(e_t.name());

    if (player1_->isCollidingWith(*entity) && (e_n.find("Fruit") != -1)) {

      player1_->setScore(player1_->getScore() + 1);
      Vector2 np = {
          (float)GetRandomValue(0, screenSize_.x - entity->getSize().x),
          (float)GetRandomValue(0, screenSize_.y - entity->getSize().y)};
      entity->setPosition((const Vector2)np);
    }

    if (player1_->isCollidingWith(*entity) && (e_n.find("Bullet") != -1) &&
        (dt >= player1_->getInvincibleTime())) {
      player1_->setHealthPoints(player1_->getHealthPoints() - 1);
      entity->shouldDestroy = true;
      if (player1_->getHealthPoints() == 0)
        state = END_SCREEN;
    }
    i++;
  }
}

void Context::renderStartScreen() {
  DrawText("Aperte P para começar!", (screenSize_.x / 2) - 300,
           screenSize_.y / 2, 50, RAYWHITE);
}

void Context::renderEndScreen() {
  DrawText("Você morreu!", (screenSize_.x / 2) - 150, (screenSize_.y / 2) - 25,
           50, RAYWHITE);
  DrawText(TextFormat("Sua Pontuação: %d", player1_->getScore()),
           (screenSize_.x / 2) - 200, (screenSize_.y / 2) + 25, 50, RAYWHITE);
  DrawText("Aperte P para recomeçar!", (screenSize_.x / 2) - 300,
           (screenSize_.y / 2) + 100, 50, RAYWHITE);
}

void Context::renderGameScreen() {

  DrawText(TextFormat("Pontos: %d", player1_->getScore()), 10, 10, 25,
           RAYWHITE);
  DrawText(TextFormat("Vidas: %d", player1_->getHealthPoints()), 10, 40, 25,
           RAYWHITE);
  for (auto entity : entities) {
    entity->draw();
  }
}

void Context::render() {
  player1_->setColor(BLACK);
  BeginDrawing();
  {

    ClearBackground(RED);

    switch (state) {
    case START_SCREEN:
      renderStartScreen();
      break;
    case END_SCREEN:
      renderEndScreen();
      break;
    case GAME_SCREEN:
      renderGameScreen();
      break;
    }
  }
  EndDrawing();
}
