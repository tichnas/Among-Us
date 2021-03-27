#include "GameLevel.h"

#include <fstream>
#include <iostream>
#include <sstream>

void GameLevel::Load(const char *file, unsigned int levelWidth,
                     unsigned int levelHeight) {
  this->Walls.clear();

  unsigned int tileCode;
  GameLevel level;
  std::string line;
  std::ifstream fstream(file);
  std::vector<std::vector<unsigned int>> tileData;

  if (fstream) {
    while (std::getline(fstream, line))  // read each line from level file
    {
      std::istringstream sstream(line);
      std::vector<unsigned int> row;
      while (sstream >> tileCode)  // read each word separated by spaces
        row.push_back(tileCode);
      tileData.push_back(row);
    }
    if (tileData.size() > 0) this->init(tileData, levelWidth, levelHeight);
  }
}

void GameLevel::Draw(SpriteRenderer &renderer) {
  for (GameObject &tile : this->Walls) tile.Draw(renderer);
}

bool GameLevel::IsCompleted() {
  // for (GameObject &tile : this->Bricks)
  //   if (!tile.IsSolid && !tile.Destroyed) return false;
  // return true;
  return false;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData,
                     unsigned int levelWidth, unsigned int levelHeight) {
  unsigned int height = tileData.size();
  unsigned int width = tileData[0].size();

  float unit_width = levelWidth / static_cast<float>(width),
        unit_height = levelHeight / height;

  std::cout << width << ' ' << height << '\n'
            << levelWidth << ' ' << levelHeight << '\n';

  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      if (tileData[y][x] == 1) {
        std::cout << unit_height * y << ' ' << unit_height << '\n';
        glm::vec2 pos(unit_width * x, unit_height * y);
        glm::vec2 size(unit_width, unit_height);
        GameObject obj(pos, size, ResourceManager::GetTexture("wall"),
                       glm::vec3(1.0f, 1.0f, 1.0f));
        this->Walls.push_back(obj);
      }
    }
  }
}
