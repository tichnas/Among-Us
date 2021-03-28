#ifndef GAME_H
#define GAME_H

#include "Game.h"

#include <queue>

#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;
glm::vec2 lastImposterMove;

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
  ResourceManager::LoadTexture("../textures/imposter.png", true, "imposter");
  this->Level.Load("../levels/one.lvl", this->Width, this->Height);
}

glm::vec2 bestMovement(std::vector<GameObject> walls, int width, int height,
                       int unitWidth, int unitHeight, GameObject* Player,
                       GameObject* Imposter) {
  width /= unitWidth;
  height /= unitHeight;
  int distance[width][height];
  bool blocked[width][height];
  std::pair<int, int> parent[width][height];
  std::pair<int, int> start = {
      (Player->Position.x + Player->Size.x / 2) / unitWidth,
      (Player->Position.y + Player->Size.y / 2) / unitHeight};
  std::pair<int, int> end = {
      (Imposter->Position.x + Player->Size.x / 2) / unitWidth,
      (Imposter->Position.y + Player->Size.y / 2) / unitHeight};

  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++) {
      distance[x][y] = 1e7;
      blocked[x][y] = false;
    }

  for (GameObject wall : walls) {
    blocked[(int)(wall.Position.x + wall.Size.x / 2) / unitWidth]
           [(int)(wall.Position.y + wall.Size.y / 2) / unitHeight] = true;
  }

  std::queue<std::pair<int, int>> q;

  parent[start.first][start.second] = start;
  q.push(start);
  distance[start.first][start.second] = 0;

  while (!q.empty()) {
    std::pair<int, int> p = q.front();
    q.pop();

    if (blocked[p.first][p.second]) continue;

    if (p.first > 0 &&
        distance[p.first - 1][p.second] > distance[p.first][p.second] + 1) {
      distance[p.first - 1][p.second] = distance[p.first][p.second] + 1;
      parent[p.first - 1][p.second] = p;
      q.push({p.first - 1, p.second});
    }

    if (p.first + 1 < width &&
        distance[p.first + 1][p.second] > distance[p.first][p.second] + 1) {
      distance[p.first + 1][p.second] = distance[p.first][p.second] + 1;
      parent[p.first + 1][p.second] = p;
      q.push({p.first + 1, p.second});
    }

    if (p.second > 0 &&
        distance[p.first][p.second - 1] > distance[p.first][p.second] + 1) {
      distance[p.first][p.second - 1] = distance[p.first][p.second] + 1;
      parent[p.first][p.second - 1] = p;
      q.push({p.first, p.second - 1});
    }

    if (p.second + 1 < height &&
        distance[p.first][p.second + 1] > distance[p.first][p.second] + 1) {
      distance[p.first][p.second + 1] = distance[p.first][p.second] + 1;
      parent[p.first][p.second + 1] = p;
      q.push({p.first, p.second + 1});
    }
  }

  std::pair<int, int> destination = parent[end.first][end.second];
  return glm::vec2((destination.first - end.first) * unitWidth * 0.01,
                   (destination.second - end.second) * unitHeight * 0.01);
}

void Game::Update(float dt) {
  if (this->State == GAME_ACTIVE) {
    GameObject* Imposter = this->Level.Imposter;
    std::vector<GameObject> walls = this->Level.Walls;
    glm::vec2 move(0.0f, 0.0f);

    if ((int)Imposter->Position.x % this->Level.UnitWidth ||
        (int)Imposter->Position.y % this->Level.UnitHeight)
      move = lastImposterMove;
    else
      move = lastImposterMove =
          bestMovement(walls, this->Width, this->Height, this->Level.UnitWidth,
                       this->Level.UnitHeight, this->Level.Player, Imposter);

    Imposter->Position += move;
  }
}

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