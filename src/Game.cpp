#ifndef GAME_H
#define GAME_H

#include "Game.h"

#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;

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
  ResourceManager::LoadTexture("../textures/wall.png", true, "wall");
  ResourceManager::LoadTexture("../textures/player.png", true, "player");
  this->Level.Load("../levels/one.lvl", this->Width, this->Height);
}

void Game::Update(float dt) {}

void Game::ProcessInput(float dt) {
  if (this->State == GAME_ACTIVE) {
    GameObject* Player = this->Level.Player;
    std::vector<GameObject> walls = this->Level.Walls;
    glm::vec2 move(0.0f, 0.0f);

    if (this->Keys[GLFW_KEY_A]) {
      move -= glm::vec2(Player->Velocity.x, 0);
    }
    if (this->Keys[GLFW_KEY_D]) {
      move += glm::vec2(Player->Velocity.x, 0);
    }
    if (this->Keys[GLFW_KEY_S]) {
      move += glm::vec2(0, Player->Velocity.y);
    }
    if (this->Keys[GLFW_KEY_W]) {
      move -= glm::vec2(0, Player->Velocity.y);
    }

    Player->Position += move;

    for (GameObject wall : walls) {
      if (Player->CheckCollision(wall)) {
        Player->Position -= move;
        break;
      }
    }
  }
}

void Game::Render() {
  Texture2D myTexture = ResourceManager::GetTexture("face");
  Renderer->DrawSprite(myTexture, glm::vec2(200.0f, 200.0f),
                       glm::vec2(300.0f, 400.0f), 45.0f,
                       glm::vec3(0.0f, 1.0f, 0.0f));
  if (this->State == GAME_ACTIVE) {
    this->Level.Draw(*Renderer);
    ;
  }
}

#endif