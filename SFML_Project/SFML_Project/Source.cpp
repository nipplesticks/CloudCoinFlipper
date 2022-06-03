#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Coin.h"

int main()
{
  sf::Font font;
  font.loadFromFile("Assets/BASKVILL.TTF");
  sf::RenderWindow wnd(sf::VideoMode(1280, 720), "CloudCoinFlipper");
  Coin::LoadTextures();
  sf::Texture background;
  background.loadFromFile("Assets/bck.png");
  sf::RectangleShape bck(sf::Vector2f(1280, 720));
  bck.setTexture(&background);
  
  Coin coin;
  coin.setPosition(1280 / 2, 720 / 1.5);
  coin.setOrigin(coin.getSize() * 0.5f);

  bool spaceWasPressed = false;

  sf::Text points;
  points.setOutlineThickness(1);
  points.setFont(font);
  int nrOfHeads = 0;
  int nrOfTails = 0;

  bool coinJustDone = false;


  sf::Text info;
  info.setFont(font);

  bool moveInfo = false;

  float moveTimer = 0.0f;
  float speed = 320.0f;

  sf::SoundBuffer flipBuffer;
  flipBuffer.loadFromFile("Assets/flipping.wav");
  sf::SoundBuffer landingBuffer;
  landingBuffer.loadFromFile("Assets/landing.wav");


  sf::Sound flipSound;
  flipSound.setBuffer(flipBuffer);
  flipSound.setLoop(true);
  sf::Sound landingSound;
  landingSound.setBuffer(landingBuffer);


  sf::Clock deltaTime;
  while (wnd.isOpen())
  {
    sf::Event e;
    while (wnd.pollEvent(e))
    {
      switch (e.type)
      {
      case sf::Event::Closed:
        wnd.close();
        break;
      default:
        break;
      }
    }
    float dt = deltaTime.restart().asSeconds();

    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    if (!coin.IsFlipping() && spacePressed && !spaceWasPressed)
    {
      flipSound.play();
      coinJustDone = true;
      moveInfo = false;
      moveTimer = 0;
      info.setString("");
      coin.setPosition(1280 / 2, 720 / 1.5);
      coin.setScale(1.0f, 1.0f);
      coin.Flip();
    }

    spaceWasPressed = spacePressed;

    coin.Update(dt);

    if (!coin.IsFlipping() && coinJustDone)
    {
      flipSound.stop();
      landingSound.play();
      coinJustDone = false;
      moveInfo = true;
      info.setPosition(coin.getPosition() + sf::Vector2f(-128 / 2, - 128 / 2));
      if (coin.IsHeads())
      {
        info.setFillColor(sf::Color::Yellow);
        speed = 320.0f;
        info.setString("HEADS!");
        nrOfHeads++;
      }
      else
      {
        info.setFillColor(sf::Color::Cyan);
        speed = -320.0f;
        info.setString("TAILS!");
        nrOfTails++;
      }
    }

    if (moveInfo)
    {
      moveTimer += dt * 3.14f;
      info.move(sin(moveTimer) *  speed * dt, -dt * 300.0f);
    }

    points.setString("NrHeads: " + std::to_string(nrOfHeads) + "\nNrTails: " + std::to_string(nrOfTails));


    wnd.clear();
    wnd.draw(bck);
    wnd.draw(points);
    wnd.draw(coin);
    wnd.draw(info);
    wnd.display();
  }

  return 0;
}