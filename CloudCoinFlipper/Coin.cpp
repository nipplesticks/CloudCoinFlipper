#include "Coin.h"

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

void Coin::LoadTextures()
{
  COIN_SHEET.loadFromFile("Assets/Coin_Spritesheet.png");
  COIN_SHEET.generateMipmap();
}

Coin::Coin()
{
  setPosition(0, 0);
  setSize(sf::Vector2f(TEXTURE_SIZE, TEXTURE_SIZE));
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
  myHasConnection = true;
  if (mySocket.connect("192.168.8.103", 4444) != sf::Socket::Done)
  {
    myHasConnection = false;
    std::cout << "Failed to connect!\n";
  }
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
        std::cout << "failed to receive!\n";
        myHasConnection = false;
      }
      int value = val.value;
      if (value == 0)
      {
        myTextureYStep = 0;
        myIsHeads = true;
      }
      else if (value == 1)
      {
        myTextureYStep = 2;
        myIsHeads = false;
      }
    }
    if (!myHasConnection)
    {
      if (myTextureYStep == 0 || myTextureYStep == 3)
      {
        myIsHeads = true;
        myTextureYStep = 0;
        std::cout << "HEADS!\n";
      }
      else
      {
        myTextureYStep = 2;
        myIsHeads = false;
        std::cout << "TAIL!\n";
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
      move(0.0f, -dt * (720.0f / MAX_FLIP_TIME) * 2.0f);
      scale(1 + dt * 0.5f, 1 + dt * 0.5f);
    }
    else
    {
      move(0.0f, dt * (720.0f / MAX_FLIP_TIME) * 2.0f);
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

void Coin::Flip()
{
  if (!myIsFlipping)
  {
    if (myHasConnection)
    {
      RandPacket rndPacket{};
      rndPacket.min = 0;
      rndPacket.max = 1;
      if (mySocket.send(&rndPacket, sizeof(rndPacket)) != sf::Socket::Done)
      {
        std::cout << "Failed to send packet!\n";
        myHasConnection = false;
      }
    }
    myFlipTimer = 0.0f;
    myIsFlipping = true;
    myFlipTime =  1.0f + ((float)(rand() % 1000 + 1) / 1000.0f) * (MAX_FLIP_TIME - 1.0f);
  }
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
