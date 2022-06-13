#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include "Coin.h"
#include "TextInput.h"
#include <fstream>
#include <iostream>
#include <functional>



struct SimpleButton : public sf::RectangleShape
{
  SimpleButton() : sf::RectangleShape()
  {
    setFillColor(sf::Color(200,200,200));
  }


  void Update(const sf::Vector2f& mousePosition)
  {
    if (ContainsMouse(mousePosition))
      setFillColor(sf::Color::White);
    else
      setFillColor(sf::Color(200, 200, 200));
  }

  bool ContainsMouse(const sf::Vector2f& mousePos)
  {
    return getGlobalBounds().contains(mousePos);
  }
};

int main()
{
  sf::Texture disconnected;
  disconnected.loadFromFile("Assets/Disconnected.png");
  sf::Texture connected;
  connected.loadFromFile("Assets/Connected.png");
  sf::Font font;
  font.loadFromFile("Assets/BASKVILL.TTF");
  sf::RenderWindow wnd(sf::VideoMode(1280, 720), "CloudCoinFlipper");
  auto wndSize = wnd.getSize();
  Coin::WND_SIZE = (sf::Vector2f)wndSize;
  Coin::LoadTextures();
  sf::Texture background;
  background.loadFromFile("Assets/bck.png");
  sf::RectangleShape bck;

  Coin coin;
  bck.setSize(Coin::WND_SIZE);
  bck.setTexture(&background);
  coin.setPosition(Coin::WND_SIZE.x / 2, Coin::WND_SIZE.y / 1.5);
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


  sf::Text pressSpace;
  pressSpace.setFont(font);
  pressSpace.setString("Press 'space' to flip");
  pressSpace.setOrigin(sf::Vector2f(pressSpace.getGlobalBounds().width, pressSpace.getGlobalBounds().height) * 0.5f);
  pressSpace.setPosition(Coin::WND_SIZE * 0.5f);
  pressSpace.setOutlineThickness(1);


  TextInput ipInput;
  ipInput.SetStrMaxLength(15);
  ipInput.SetTextBoxName("IP");
  ipInput.setSize(sf::Vector2f(250, 50));
  ipInput.setPosition(Coin::WND_SIZE.x - 250, 0);
  
  TextInput portInput;
  portInput.SetStrMaxLength(5);
  portInput.SetTextBoxName("PORT");
  portInput.setSize(sf::Vector2f(100, 50));
  portInput.setPosition(Coin::WND_SIZE.x - 100, 55);

  SimpleButton connectButton;
  connectButton.setPosition(portInput.getPosition() + sf::Vector2f(0, portInput.getSize().y + 5));
  connectButton.setSize(sf::Vector2f(100, 50));
  connectButton.setTexture(&disconnected);

  std::ifstream input;
  input.open("Assets/IpAndPort.txt");
  if (input)
  {
    std::string ip;
    std::getline(input, ip);
    unsigned short port;
    input >> port;
    ipInput.SetString(ip);
    portInput.SetString(std::to_string(port));
    input.close();
  }

  bool mouseWasPressed = false;

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
      case sf::Event::Resized:
      {
        Coin::WND_SIZE.x = e.size.width;
        Coin::WND_SIZE.y = e.size.height;
        sf::FloatRect va(0.0f, 0.0f, (float)e.size.width, (float)e.size.height);
        wnd.setView(sf::View(va));
        bck.setSize(Coin::WND_SIZE);
        coin.setPosition(Coin::WND_SIZE.x / 2, Coin::WND_SIZE.y / 1.5);
        pressSpace.setPosition(Coin::WND_SIZE * 0.5f);
        ipInput.setPosition(Coin::WND_SIZE.x - 250, 0);
        portInput.setPosition(Coin::WND_SIZE.x - 100, 55);
        connectButton.setPosition(portInput.getPosition() + sf::Vector2f(0, portInput.getSize().y + 5));
        break;
      }
      case sf::Event::TextEntered:
      {
        sf::Uint32 c = e.text.unicode;
        ipInput.TextEntered(c);
        portInput.TextEntered(c);
      }
      case sf::Event::KeyPressed:
      {
        sf::Keyboard::Key key = e.key.code;
        if (key == sf::Keyboard::Delete || key == sf::Keyboard::Home || key == sf::Keyboard::End ||
          key == sf::Keyboard::Left || key == sf::Keyboard::Right)
        {
          ipInput.TextEntered(key, true);
          portInput.TextEntered(key, true);
        }
      }
      default:
        break;
      }
    }

    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(wnd);
    bool mousePress = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    float dt = deltaTime.restart().asSeconds();

    if (!mousePress && mouseWasPressed && wnd.hasFocus())
    {
      ipInput.OnMousePress(mousePosition);
      portInput.OnMousePress(mousePosition);
      if (connectButton.ContainsMouse(mousePosition))
      {
        if (!coin.IsConnected())
        {
          std::cout << "Connecting...\n";
          std::string ip = ipInput.GetString();
          unsigned int port = atoi(portInput.GetString().c_str());
          std::ofstream output;
          output.open("Assets/IpAndPort.txt");
          if (output)
          {
            output << ip << "\n";
            output << port << "\n";;
            output.close();
          }
          coin.Connect(ipInput.GetString(), atoi(portInput.GetString().c_str()));
        }
        else
        {
          std::cout << "Disconnecting...\n";
          coin.Disconnect();
        }

        if (coin.IsConnected())
          connectButton.setTexture(&connected);
        else
          connectButton.setTexture(&disconnected);
      }
    }
    mouseWasPressed = mousePress;

    ipInput.Update(dt);
    portInput.Update(dt);
    connectButton.Update(mousePosition);


    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && wnd.hasFocus() && !portInput.IsActive() && !ipInput.IsActive();

    if (!coin.IsFlipping() && spacePressed && !spaceWasPressed)
    {
      flipSound.play();
      coinJustDone = true;
      moveInfo = false;
      moveTimer = 0;
      info.setString("");
      coin.setPosition(Coin::WND_SIZE.x / 2, Coin::WND_SIZE.y / 1.5);
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
      info.setPosition(coin.getPosition() + sf::Vector2f(-128 / 2, -128 / 2));
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
      info.move(sin(moveTimer) * speed * dt, -dt * 300.0f);
    }

    points.setString("NrHeads: " + std::to_string(nrOfHeads) + "\nNrTails: " + std::to_string(nrOfTails));


    wnd.clear();
    wnd.draw(bck);
    wnd.draw(points);
    wnd.draw(coin);
    ipInput.Draw(&wnd);
    portInput.Draw(&wnd);
    wnd.draw(connectButton);
    if (!coinJustDone)
      wnd.draw(pressSpace);
    wnd.draw(info);
    wnd.display();
  }

  return 0;
}