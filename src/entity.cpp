#include "entity.h"
#include "context.h"
#include "settings.h"
#include <ctime>
#include <iostream>
#include <raylib.h>
#include <raymath.h>

using namespace steelcpp;

Entity::Entity() {}

Entity::~Entity() {}

EntitySize_t Entity::getSize() const { return size_; }

Vector2 Entity::getPosition() const { return position_; }

void Entity::setSize(const EntitySize_t size) {
  size_.x = size.x;
  size_.y = size.y;
}

void Entity::setPosition(const Vector2 position) {
  position_.x = position.x;
  position_.y = position.y;
}
/*
void Entity::draw() {}

void Entity::update() {}
*/

bool Entity::isCollidingWith(Entity &entity) { return false; }

Player::Player(Context *context, std::string name, PlayerSize_t size,
               Vector2 position, Vector2 speed,
               PlayerHealthPoints_t healthPoints, Color color) {
  context_ = context;
  name_ = name;
  size_ = size;
  position_ = position;
  speed_ = speed;
  healthPoints_ = healthPoints;
  score_ = 0;
  color_ = color;
  movement_ = STOP;
}

Player::~Player() {}

PlayerHealthPoints_t Player::getHealthPoints() const { return healthPoints_; }

PlayerMovement Player::getMovement() const { return movement_; }

std::string Player::getName() const { return name_; }

PlayerScore_t Player::getScore() const { return score_; }

Vector2 Player::getSpeed() const { return speed_; }

float Player::getInvincibleTime() const { return invincibleTime_; }

void Player::setColor(const Color color) {
  color_.r = color.r;
  color_.g = color.g;
  color_.b = color.b;
  color_.a = color.a;
}

void Player::setHealthPoints(const PlayerHealthPoints_t healthPoints) {
  healthPoints_ = healthPoints;
}

void Player::setMovement(const PlayerMovement movement) {
  movement_ = movement;
}

void Player::setScore(const PlayerScore_t score) { score_ = score; }

void Player::setSpeed(const Vector2 speed) {
  speed_.x = speed.x;
  speed_.y = speed.y;
}

void Player::setInvincibleTime(const float it) { invincibleTime_ = it; }

void Player::update() { updateMovement(); }

void Player::updateMovement() {
  Vector2 speed = speed_;
  if ((movement_ & MOVE_RIGHT) == MOVE_RIGHT) {
    if (movement_ - MOVE_RIGHT > 0)
      speed.x = speed_.x * 3 / 4;

    position_.x += speed.x * context_->getDeltaFrameTime() / BILLION;
  } else if ((movement_ & MOVE_LEFT) == MOVE_LEFT) {
    if (movement_ - MOVE_LEFT > 0)
      speed.x = speed_.x * 3 / 4;

    position_.x += -speed.x * context_->getDeltaFrameTime() / BILLION;
  }

  if ((movement_ & MOVE_UP) == MOVE_UP) {
    if (movement_ - MOVE_UP > 0)
      speed.y = speed_.y * 3 / 4;

    position_.y += -speed.y * context_->getDeltaFrameTime() / BILLION;
  } else if ((movement_ & MOVE_DOWN) == MOVE_DOWN) {
    if (movement_ - MOVE_DOWN > 0)
      speed.y = speed_.y * 3 / 4;

    position_.y += speed.y * context_->getDeltaFrameTime() / BILLION;
  }

  Vector2 screenSize = context_->getScreenSize();
  if (position_.x > screenSize.x)
    position_.x = 0;
  else if (position_.x + size_.x < 0)
    position_.x = screenSize.x;

  if (position_.y > screenSize.y)
    position_.y = 0;
  else if (position_.y + size_.y < 0)
    position_.y = screenSize.y;
}

void Player::draw() {
  DrawRectangle(position_.x, position_.y, size_.x, size_.y, color_);
}

void Player::handleKeyPressed() {
  int key = GetKeyPressed();
  while (key > 0) {
    switch (key) {
    case KEY_RIGHT:
      movement_ = (movement_ & ~MOVE_LEFT) | MOVE_RIGHT;
      break;
    case KEY_LEFT:
      movement_ = (movement_ & ~MOVE_RIGHT) | MOVE_LEFT;
      break;
    case KEY_UP:
      movement_ = (movement_ & ~MOVE_DOWN) | MOVE_UP;
      break;
    case KEY_DOWN:
      movement_ = (movement_ & ~MOVE_UP) | MOVE_DOWN;
      break;
    default:
      break;
    }
    key = GetKeyPressed();
  }
}

void Player::handleKeyReleased() {
  if (IsKeyReleased(KEY_RIGHT)) {
    movement_ = movement_ & ~MOVE_RIGHT;
  } else if (IsKeyReleased(KEY_LEFT)) {
    movement_ = movement_ & ~MOVE_LEFT;
  }

  if (IsKeyReleased(KEY_UP)) {
    movement_ = movement_ & ~MOVE_UP;
  } else if (IsKeyReleased(KEY_DOWN)) {
    movement_ = movement_ & ~MOVE_DOWN;
  }

  if (movement_ == 0)
    movement_ = STOP;
}

bool Player::isCollidingWith(Entity &entity) {
  Vector2 ep = entity.getPosition();
  Vector2 es = entity.getSize();
  if (position_.x < ep.x + es.x && position_.x + size_.x > ep.x &&
      position_.y + size_.y > ep.y && position_.y < ep.y + es.y) {
    return true;
  }

  return false;
}

Bullet::Bullet(Context *context, BulletSize_t size, Vector2 position,
               Vector2 destination, Color color, Vector2 speed) {
  context_ = context;
  size_ = size;
  position_ = position;
  color_ = color;
  speed_ = Vector2Subtract(destination, position);
  speed_ = Vector2Multiply(Vector2Normalize(speed_), speed);
  fprintf(stdout, "Speed:\nx  = %f\ny = %f\n", speed_.x, speed_.y);
}

Bullet::~Bullet() {}

void Bullet::update() {
  if (position_.x >= context_->getScreenSize().x ||
      position_.x + size_.x <= 0 ||
      position_.y >= context_->getScreenSize().y ||
      position_.y + size_.y <= 0) {
    shouldDestroy = true;
  }
  position_ = Vector2Add(
      position_, Vector2Scale(speed_, context_->getDeltaFrameTime() / BILLION));
}

void Bullet::draw() {
  DrawRectangle(position_.x, position_.y, size_.x, size_.y, color_);
}

void Bullet::setColor(const Color color) { color_ = color; }

Color Bullet::getColor() const { return color_; }

Fruit::Fruit(Context *context, std::string name, EntitySize_t size,
             Vector2 position, Color color, float reloadTime) {
  context_ = context;
  name_ = name;
  size_ = size;
  position_ = position;
  color_ = color;
  reloadTime_ = reloadTime;
  lastShootTime.tv_sec = 0;
  lastShootTime.tv_nsec = 0;
}

Fruit::~Fruit() {}

void Fruit::update() {
  Player player = context_->getPlayer();
  // printf("Fruit Update\n");
  this->shoot(player.getPosition());
}

void Fruit::draw() {
  DrawRectangle(position_.x, position_.y, size_.x, size_.y, color_);
}

std::string Fruit::getName() const { return name_; }

Color Fruit::getColor() const { return color_; }

float Fruit::getReloadTime() const { return reloadTime_; }

void Fruit::setColor(const Color color) {
  color_.r = color.r;
  color_.g = color.g;
  color_.b = color.b;
  color_.a = color.a;
}

void Fruit::setReloadTime(const float reloadTime) { reloadTime_ = reloadTime; }

void Fruit::shoot(Vector2 dest) {

  struct timespec currentTime;
  clock_gettime(CLOCK_MONOTONIC, &currentTime);
  float ctSec =
      (float)currentTime.tv_sec + (float)currentTime.tv_nsec / BILLION;
  float ltSec =
      (float)lastShootTime.tv_sec + (float)lastShootTime.tv_nsec / BILLION;

  if ((ctSec - ltSec) >= reloadTime_) {
    Bullet *bullet =
        new Bullet(context_, {10, 10}, position_, dest, YELLOW, {250, 250});
    context_->entities.push_back(static_cast<Entity *>(bullet));
    // fprintf(stdout, "Entities: %ld\n", context_->entities.size());
    clock_gettime(CLOCK_MONOTONIC, &lastShootTime);
  }
}

bool Fruit::isCollidingWith(Entity &entity) {

  Vector2 ep = entity.getPosition();
  Vector2 es = entity.getSize();
  if (position_.x < ep.x + es.x && position_.x + size_.x > ep.x &&
      position_.y + size_.y > ep.y && position_.y < ep.y + es.y) {
    return true;
  }

  return false;
}
