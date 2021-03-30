#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <sstream>

#include "GameLevel.h"

enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };

class Game {
 public:
  GameState State;
  GameLevel Level;
  bool Keys[1024];
  unsigned int Width, Height;
  int Score;
  float Time;

  Game(unsigned int width, unsigned int height);
  ~Game();

  void Init();
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();
};