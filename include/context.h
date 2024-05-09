#ifndef CONTEXT_H_
#define CONTEXT_H_
#include <chrono>
#include <raylib.h>
#include <string>
#include <vector>
#define EXPORT_FN

namespace steelcpp {

class Entity;
class Player;
class Fruit;

typedef enum State {
  START_SCREEN = 1,
  END_SCREEN = 2,
  GAME_SCREEN = 4,
} State;

class Context {
public:
  State state;
  bool isRunning;
  std::vector<Entity *> entities;
  std::vector<Fruit *> fruits;

public:
  int setup();
  void run();
  Context(Vector2 screenSize, const char *windowTitle);
  ~Context();
  Vector2 getScreenSize() const;
  long getDeltaFrameTime() const;
  Player &getPlayer();
  void calculateDeltaFrameTime();

  void update();
  void processInput();
  void updateEntities();
  void render();
  void renderGameScreen();
  void renderStartScreen();
  void renderEndScreen();

private:
  struct timespec startTime;
  Player *player1_;
  Vector2 screenSize_;
  std::string windowTitle_;
  std::chrono::steady_clock::time_point lastFrameTime_;
  long deltaFrameTime_ = 0;
};

} // namespace steelcpp

#endif
