#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "SpriteRenderer.h"
#include "Texture.h"

class GameObject {
 public:
  glm::vec2 Position, Size, Velocity;
  glm::vec3 Color;
  float Rotation;
  Texture2D Sprite;

  GameObject();
  GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite,
             glm::vec3 color = glm::vec3(1.0f),
             glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

  void UpdateSprite(Texture2D sprite);

  virtual void Draw(SpriteRenderer &renderer);
  bool CheckCollision(GameObject obj);
};

#endif
