#ifndef ENTITY_H_
#define ENTITY_H_

#include <bits/types/struct_timespec.h>
#include <raylib.h>
#include <string>

namespace steelcpp {

class Context;

typedef Vector2 EntitySize_t;

typedef uint8_t PlayerHealthPoints_t;
typedef uint16_t PlayerScore_t;
typedef int PlayerMovementFlag_t;
typedef EntitySize_t PlayerSize_t;

enum PlayerMovement : int {
  STOP = 0,
  MOVE_LEFT = 1,
  MOVE_RIGHT = 2,
  MOVE_UP = 4,
  MOVE_DOWN = 8
};

inline PlayerMovement operator~(PlayerMovement a) {
  return static_cast<PlayerMovement>(~(static_cast<int>(a)));
}

inline PlayerMovement operator|(PlayerMovement a, PlayerMovement b) {
  return static_cast<PlayerMovement>(static_cast<int>(a) | static_cast<int>(b));
}

inline PlayerMovement operator&(PlayerMovement a, PlayerMovement b) {
  return static_cast<PlayerMovement>(static_cast<int>(a) & static_cast<int>(b));
}

typedef EntitySize_t FruitSize_t;
typedef EntitySize_t BulletSize_t;

class Entity {
public:
  bool shouldDestroy = false;

public:
  Entity();
  virtual ~Entity() = 0;

  bool isCollidingWith(Entity &entity);
  EntitySize_t getSize() const;
  Vector2 getPosition() const;

  void setSize(const EntitySize_t size);
  void setPosition(const Vector2 position);

  virtual void update() = 0;
  virtual void draw() = 0;

protected:
  Context *context_;
  Vector2 position_;
  EntitySize_t size_;
};

class Player : public Entity {
public:
  Player(Context *context, std::string name, PlayerSize_t size,
         Vector2 position, Vector2 speed, PlayerHealthPoints_t healthPoints,
         Color color);
  ~Player();
  void update();
  void draw();
  void handleKeyPressed();
  void handleKeyReleased();
  void updateMovement();
  bool isCollidingWith(Entity &entity);

  std::string getName() const;
  Vector2 getSpeed() const;
  PlayerScore_t getScore() const;
  PlayerHealthPoints_t getHealthPoints() const;
  PlayerMovement getMovement() const;
  Color getColor() const;
  float getInvincibleTime()const;

  void setSpeed(const Vector2 speed);
  void setScore(const PlayerScore_t score);
  void setHealthPoints(const PlayerHealthPoints_t healthPoints);
  void setMovement(const PlayerMovement movement);
  void setColor(const Color color);
  void setInvincibleTime(const float it);

private:
  float invincibleTime_;
  Color color_;
  PlayerMovement movement_;
  std::string name_;
  Vector2 speed_;
  PlayerScore_t score_ = 0;
  PlayerHealthPoints_t healthPoints_ = 0;
};

class Bullet : public Entity {
public:
  Bullet(Context *context, BulletSize_t size, Vector2 position,
         Vector2 destination, Color color, Vector2 speed);
  ~Bullet();
  void update();
  void draw();

  Color getColor() const;

  void setColor(const Color color);
private:
    Color  color_;
    Vector2 speed_;

};

class Fruit : public Entity {
public:
  struct timespec lastShootTime;

public:
  Fruit(Context *context, std::string name, FruitSize_t size, Vector2 position,
        Color color, float reloadTime);
  ~Fruit();
  void update();
  void draw();
  void shoot(Vector2 dest);

  std::string getName() const;
  Color getColor() const;
  float getReloadTime()const;

  bool isCollidingWith(Entity &entity);

  void setColor(const Color color);
  void setReloadTime(const float reloadTime);
private:
  float reloadTime_;
  Color color_;
  std::string name_;
};

} // namespace steelcpp
#endif
