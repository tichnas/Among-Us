#ifndef GAME_H
#define GAME_H

#include "Game.h"

#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer *Renderer;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {}

Game::~Game() { delete Renderer; }

void Game::Init() {
  // load shaders
  ResourceManager::LoadShader("../src/shaders/sprite.vs",
                              "../src/shaders/sprite.frag", NULL, "sprite");
  // configure shaders
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(this->Width),
                 static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
  ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
  // set render-specific controls
  Shader myShader = ResourceManager::GetShader("sprite");
  Renderer = new SpriteRenderer(myShader);
  // load textures
  ResourceManager::LoadTexture("../textures/awesomeface.png", true, "face");
}

void Game::Update(float dt) {}

void Game::ProcessInput(float dt) {}

void Game::Render() {
  Texture2D myTexture = ResourceManager::GetTexture("face");
  Renderer->DrawSprite(myTexture, glm::vec2(200.0f, 200.0f),
                       glm::vec2(300.0f, 400.0f), 45.0f,
                       glm::vec3(0.0f, 1.0f, 0.0f));
}

#endif