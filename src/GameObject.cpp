#include "GameObject.h"

GameObject::GameObject()
    : Position(0.0f, 0.0f),
      Size(1.0f, 1.0f),
      Velocity(0.0f),
      Color(1.0f),
      Rotation(0.0f),
      Sprite() {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite,
                       glm::vec3 color, glm::vec2 velocity)
    : Position(pos),
      Size(size),
      Velocity(velocity),
      Color(color),
      Rotation(0.0f),
      Sprite(sprite) {}

void GameObject::Draw(SpriteRenderer &renderer) {
  renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation,
                      this->Color);
}

bool GameObject::CheckCollision(GameObject obj) {
  bool collisionX = this->Position.x + this->Size.x > obj.Position.x &&
                    obj.Position.x + obj.Size.x > this->Position.x;
  bool collisionY = this->Position.y + this->Size.y > obj.Position.y &&
                    obj.Position.y + obj.Size.y > this->Position.y;

  return collisionX && collisionY;
}