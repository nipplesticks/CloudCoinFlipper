#include "Console.h"
#include "Coin.h"
#include <fstream>
#include <iostream>

struct RandPacket
{
  unsigned short min;
  unsigned short max;
};
struct ResPacket
{
  unsigned short value;
};

sf::Texture Coin::COIN_SHEET;
float Coin::SPRITE_SWAP_TIME = 0.01f;
float Coin::MAX_FLIP_TIME = 2.0f;
sf::Vector2f Coin::WND_SIZE(1280, 720);

void Coin::LoadTextures()
{
  COIN_SHEET.loadFromFile("Assets/Coin_Spritesheet.png");
  COIN_SHEET.generateMipmap();
}

Coin::Coin() : sf::RectangleShape()
{
  setPosition(0, 0);
  setSize(sf::Vector2f(TEXTURE_SIZE * 1.5f, TEXTURE_SIZE * 1.5f));
  myIsHeads = true;
  myIsFlipping = false;
  myTextureXStep = 4;
  myTextureYStep = 0;
  myTextureRect.left = myTextureXStep * TEXTURE_SIZE;
  myTextureRect.top = myTextureYStep * TEXTURE_SIZE;
  myTextureRect.width = TEXTURE_SIZE;
  myTextureRect.height = TEXTURE_SIZE;
  mySpriteTime = 0.0f;
  setTextureRect(myTextureRect);
  setTexture(&COIN_SHEET);
  myHasConnection = false;
}

Coin::~Coin()
{
  mySocket.disconnect();
}

void Coin::SetFlip(bool isFlipping)
{
  if (myIsFlipping && !isFlipping)
  {
    myTextureXStep = 4;
    if (myHasConnection)
    {
      ResPacket val{};
      size_t bytes = 0;
      if (mySocket.receive(&val, sizeof(val), bytes))
      {
        TO_CONSOLE("failed to receive!");
        myHasConnection = false;
      }
      int value = val.value;
      TO_CONSOLE("value received: %d", value);
      if (value % 2)
      {
        myTextureYStep = 0;
        myIsHeads = true;
      }
      else
      {
        myTextureYStep = 2;
        myIsHeads = false;
      }
    }
    if (!myHasConnection)
    {
      TO_CONSOLE("value localy generated: %d", (int)myFakePacket);
      if (myFakePacket % 2)
      {
        myTextureYStep = 0;
        myIsHeads = true;
      }
      else
      {
        myTextureYStep = 2;
        myIsHeads = false;
      }
    }

    myTextureRect.left = myTextureXStep * TEXTURE_SIZE;
    myTextureRect.top = myTextureYStep * TEXTURE_SIZE;
    setTextureRect(myTextureRect);
  }
  myIsFlipping = isFlipping;
}

void Coin::Update(float dt)
{
  if (myIsFlipping)
  {
    myFlipTimer += dt;

    if (myFlipTimer < myFlipTime * 0.5f)
    {
      move(0.0f, -dt * (WND_SIZE.y / MAX_FLIP_TIME) * 2.0f);
      scale(1 + dt * 0.5f, 1 + dt * 0.5f);
    }
    else
    {
      move(0.0f, dt * (WND_SIZE.y / MAX_FLIP_TIME) * 2.0f);
      scale(1 - dt * 0.5f, 1 - dt * 0.5f);
    }

    mySpriteTime += dt;

    while (mySpriteTime >= SPRITE_SWAP_TIME)
    {
      mySpriteTime -= SPRITE_SWAP_TIME;
      StepTextureRect();
    }
    if (myFlipTimer > myFlipTime)
    {
      SetFlip(false);
    }
  }
}

bool Coin::IsHeads()
{
  return myIsHeads;
}

bool Coin::IsFlipping()
{
  return myIsFlipping;
}

void Coin::Flip(unsigned short _min, unsigned short _max)
{
  if (!myIsFlipping)
  {
    if (myHasConnection)
    {
      RandPacket rndPacket{};
      rndPacket.min = _min;
      rndPacket.max = _max;

      if (mySocket.send(&rndPacket, sizeof(rndPacket)) != sf::Socket::Done)
      {
        TO_CONSOLE("Failed to send packet!");
        myHasConnection = false;
      }
      else
      {
        TO_CONSOLE("packet sent [min: %d max: %d]", _min, _max);
      }
    }
    else
    {
      TO_CONSOLE("No connection, using local [min: %d max: %d]", _min, _max);
    }
    if (_min > _max)
      std::swap(_min, _max);
    myFakePacket = rand() % (_max - _min + 1) + _min;

    myFlipTimer = 0.0f;
    myIsFlipping = true;
    myFlipTime =  1.0f + ((float)(rand() % 1000 + 1) / 1000.0f) * (MAX_FLIP_TIME - 1.0f);
  }
}

void Coin::Connect(const std::string& ip, unsigned short port)
{
  myHasConnection = true;

  if (mySocket.connect(ip, port) != sf::Socket::Done)
  {
    myHasConnection = false;
    TO_CONSOLE("Failed to connect!");
  }
}

void Coin::Disconnect()
{
  mySocket.disconnect();
  myHasConnection = false;
}

bool Coin::IsConnected()
{
  return myHasConnection;
}

void Coin::StepTextureRect()
{
  if (++myTextureXStep == TEXTURE_SHEET_WIDTH)
  {
    myTextureXStep = 0;
    myTextureYStep++;
  }
  if (myTextureYStep == TEXTURE_SHEET_HEIGHT)
  {
    myTextureYStep = 0;
  }
  myTextureRect.left = myTextureXStep * TEXTURE_SIZE;
  myTextureRect.top = myTextureYStep * TEXTURE_SIZE;
  setTextureRect(myTextureRect);
}
