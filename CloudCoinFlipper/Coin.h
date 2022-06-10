#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#define TEXTURE_SHEET_WIDTH 10
#define TEXTURE_SHEET_HEIGHT 4
#define TEXTURE_SIZE 128

class Coin : public sf::RectangleShape
{
public:
  static void LoadTextures();
  static float SPRITE_SWAP_TIME;
  static float MAX_FLIP_TIME;
  static sf::Vector2f WND_SIZE;

  Coin();
  ~Coin();

  void SetFlip(bool isFlipping);

  void Update(float dt);

  bool IsHeads();
  bool IsFlipping();
  void Flip();

private:
  static sf::Texture COIN_SHEET;
  static sf::Texture COIN_SHEET_BLUR;

  void StepTextureRect();

  sf::IntRect myTextureRect;
  bool myIsHeads;
  bool myIsFlipping;
  float mySpriteTime;
  int myTextureXStep;
  int myTextureYStep;
  float myFlipTime = 0.0f;
  float myFlipTimer = 0.0f;
  bool myHasConnection;
  sf::TcpSocket mySocket;
};