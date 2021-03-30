#ifndef GAME_H
#define GAME_H

#include "Game.h"

#include <queue>

#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"

SpriteRenderer* Renderer;
TextRenderer* Text;
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
  // setup text
  Text = new TextRenderer(this->Width, this->Height);
  Text->Load("../fonts/arial.ttf", 24);
  // load textures
  ResourceManager::LoadTexture("../textures/wall.png", true, "wall");
  ResourceManager::LoadTexture("../textures/player.png", true, "player");
  ResourceManager::LoadTexture("../textures/imposter.png", true, "imposter");
  ResourceManager::LoadTexture("../textures/vaporise.png", true, "vaporise");
  ResourceManager::LoadTexture("../textures/release.png", true, "release");
  ResourceManager::LoadTexture("../textures/coin.png", true, "power");
  ResourceManager::LoadTexture("../textures/rock.png", true, "obstacle");
  this->Level.Load("../levels/one.lvl", this->Width, this->Height);

  this->Score = 0;
  this->Time = 10;
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
    this->Time -= dt;
    if (this->Time <= 0) exit(0);

    GameObject* Imposter = this->Level.Imposter;
    GameObject* Player = this->Level.Player;
    GameObject* VaporiseBtn = this->Level.VaporiseBtn;
    GameObject* ReleaseBtn = this->Level.ReleaseBtn;

    if (Imposter) {
      std::vector<GameObject> walls = this->Level.Walls;
      glm::vec2 move(0.0f, 0.0f);

      if ((int)Imposter->Position.x % this->Level.UnitWidth ||
          (int)Imposter->Position.y % this->Level.UnitHeight)
        move = lastImposterMove;
      else
        move = lastImposterMove = bestMovement(
            walls, this->Width, this->Height, this->Level.UnitWidth,
            this->Level.UnitHeight, Player, Imposter);

      Imposter->Position += 0.05f * move;

      if (Imposter->CheckCollision(*Player)) {
        exit(0);
      }
    }

    if (VaporiseBtn) {
      if (Player->CheckCollision(*VaporiseBtn)) {
        this->Level.Imposter = NULL;
        this->Level.VaporiseBtn = NULL;
      }
    }

    if (ReleaseBtn) {
      if (Player->CheckCollision(*ReleaseBtn)) {
        this->Level.ReleaseBtn = NULL;
      }
    } else {
      std::vector<GameObject>* powers = &(this->Level.Powers);
      std::vector<GameObject>* obstacles = &(this->Level.Obstacles);

      bool check = true;

      while (check) {
        check = false;
        for (int i = 0; i < (*powers).size(); i++) {
          if (Player->CheckCollision((*powers)[i])) {
            check = true;
            (*powers).erase((*powers).begin() + i);
            this->Score += 10;
            break;
          }
        }
      }

      check = true;

      while (check) {
        check = false;
        for (int i = 0; i < (*obstacles).size(); i++) {
          if (Player->CheckCollision((*obstacles)[i])) {
            check = true;
            (*obstacles).erase((*obstacles).begin() + i);
            this->Score -= 10;
            break;
          }
        }
      }
    }
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

    move *= 0.05f;

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
  if (this->State == GAME_ACTIVE) {
    this->Level.Draw(*Renderer);

    int tasks = 2;
    if (this->Level.ReleaseBtn) tasks--;
    if (this->Level.VaporiseBtn) tasks--;

    std::stringstream ss;
    ss << "Score: ";
    ss << this->Score;
    ss << "  |  ";
    ss << "Tasks: ";
    ss << tasks;
    ss << "/2";
    ss << "  |  ";
    ss << "Time: ";
    ss << (int)this->Time;
    Text->RenderText(ss.str(), 5.0f, 5.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
  }
}

#endif