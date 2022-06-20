#include <SFML/Graphics.hpp>
#include <Windows.h>

void HideConsole()
{
  ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
  ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool IsConsoleVisible()
{
  return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}

#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include "Coin.h"
#include "TextInput.h"
#include <fstream>
#include <iostream>
#include <functional>
#include "Console.h"

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

struct IconAnimator
{
  IconAnimator()
  {
    sf::Image img;
    img.loadFromFile("Assets/Coin_Spritesheet.png");
    sf::IntRect ir;
    ir.top = 0;
    ir.left = 0;
    ir.width = TEXTURE_SIZE;
    ir.height = TEXTURE_SIZE;
    for (int y = 0; y < TEXTURE_SHEET_HEIGHT; y++)
    {
      ir.top = y * TEXTURE_SIZE;
      for (int x = 0; x < TEXTURE_SHEET_WIDTH; x++)
      {
        ir.left = x * TEXTURE_SIZE;
        int idx = x + y * TEXTURE_SHEET_WIDTH;
        myImages[idx].create(TEXTURE_SIZE, TEXTURE_SIZE);
        myImages[idx].copy(img, 0, 0, ir);
      }
    }
  }
  void Update(float dt)
  {
    myIconSwapTimer += dt;

    while (myIconSwapTimer >= mySpeed)
    {
      myUpdateReady = true;
      myIconSwapTimer -= mySpeed;
      if (++myIconXIdx == TEXTURE_SHEET_WIDTH)
      {
        myIconXIdx = 0;
        myIconYIdx++;
      }
      if (myIconYIdx == TEXTURE_SHEET_HEIGHT)
      {
        myIconYIdx = 0;
      }
    }
  }

  void GetIcon(sf::RenderWindow* wnd_p)
  {
    if (myUpdateReady)
    {
      wnd_p->setIcon(TEXTURE_SIZE, TEXTURE_SIZE, myImages[myIconXIdx + myIconYIdx * TEXTURE_SHEET_WIDTH].getPixelsPtr());
      myUpdateReady = false;
    }
  }

  sf::Image myImages[TEXTURE_SHEET_WIDTH * TEXTURE_SHEET_HEIGHT];
  float myIconSwapTimer = 0.f;
  float mySpeed = 0.15f;
  int myIconXIdx = 0;
  int myIconYIdx = 0;
  bool myUpdateReady = true;
  
};


int main()
{
  HideConsole();
  Console::Init();
  Console::GetInstance()->setSize(sf::Vector2f(512, 300));
  Console::GetInstance()->SetCharSize(20);
  sf::Texture disconnected;
  disconnected.loadFromFile("Assets/Disconnected.png");
  sf::Texture connected;
  connected.loadFromFile("Assets/Connected.png");
  sf::Font font;
  font.loadFromFile("Assets/BASKVILL.TTF");
  sf::RenderWindow wnd(sf::VideoMode(1280, 720), "CloudCoinFlipper");
  IconAnimator ia;
  ia.GetIcon(&wnd);

  auto wndSize = wnd.getSize();
  Coin::WND_SIZE = (sf::Vector2f)wndSize;
  Coin::LoadTextures();
  sf::Texture background;
  background.loadFromFile("Assets/bck.png");
  sf::RectangleShape bck;

  bck.setSize(Coin::WND_SIZE);
  bck.setTexture(&background);
  Coin coin;
  
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
  ipInput.setPosition(Coin::WND_SIZE.x - ipInput.getSize().x, 0);
  
  TextInput portInput;
  portInput.SetStrMaxLength(5);
  portInput.SetTextBoxName("PORT");
  portInput.setSize(sf::Vector2f(100, 50));
  portInput.setPosition(Coin::WND_SIZE.x - portInput.getSize().x, ipInput.getPosition().y + ipInput.getSize().y + 5);

  SimpleButton connectButton;
  connectButton.setSize(sf::Vector2f(100, 50));
  connectButton.setTexture(&disconnected);
  connectButton.setPosition(Coin::WND_SIZE.x - connectButton.getSize().x, portInput.getPosition().y + portInput.getSize().y + 5);

  TextInput minInput;
  minInput.SetStrMaxLength(5);
  minInput.SetTextBoxName("minValue");
  minInput.setSize(sf::Vector2f(100, 50));
  minInput.setPosition(Coin::WND_SIZE.x - minInput.getSize().x, connectButton.getPosition().y + connectButton.getSize().y + 5);

  TextInput maxInput;
  maxInput.SetStrMaxLength(5);
  maxInput.SetTextBoxName("maxValue");
  maxInput.setSize(sf::Vector2f(100, 50));
  maxInput.setPosition(Coin::WND_SIZE.x - maxInput.getSize().x, minInput.getPosition().y + minInput.getSize().y + 5);

  Console::GetInstance()->setPosition(0, Coin::WND_SIZE.y - Console::GetInstance()->getSize().y);

  std::ifstream input;
  input.open("Assets/IpAndPort.txt");
  if (input)
  {
    std::string ip;
    std::getline(input, ip);
    unsigned short port = UINT16_MAX;
    input >> port;
    ipInput.SetString(ip);
    portInput.SetString(std::to_string(port));
    input.close();
  }

  std::ifstream minMax;
  minMax.open("Assets/minMaxValue.txt");
  if (minMax)
  {
    unsigned short minVal = 0;
    unsigned short maxVal = 1;

    minMax >> minVal;
    minMax >> maxVal;

    minInput.SetString(std::to_string(minVal));
    maxInput.SetString(std::to_string(maxVal));

    minMax.close();
  }

  bool mouseWasPressed = false;

  bool connectButtonPressed = false;

  sf::Clock deltaTime;
  while (wnd.isOpen())
  {
    connectButtonPressed = false;
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
        ipInput.setPosition(Coin::WND_SIZE.x - ipInput.getSize().x, 0);
        portInput.setPosition(Coin::WND_SIZE.x - portInput.getSize().x, ipInput.getPosition().y + ipInput.getSize().y + 5);
        connectButton.setPosition(Coin::WND_SIZE.x - connectButton.getSize().x, portInput.getPosition().y + portInput.getSize().y + 5);
        minInput.setPosition(Coin::WND_SIZE.x - minInput.getSize().x, connectButton.getPosition().y + connectButton.getSize().y + 5);
        maxInput.setPosition(Coin::WND_SIZE.x - maxInput.getSize().x, minInput.getPosition().y + minInput.getSize().y + 5);
        Console::GetInstance()->setPosition(0, Coin::WND_SIZE.y - Console::GetInstance()->getSize().y);
        break;
      }
      case sf::Event::TextEntered:
      {
        sf::Uint32 c = e.text.unicode;
        ipInput.TextEntered(c);
        portInput.TextEntered(c);
        minInput.TextEntered(c);
        maxInput.TextEntered(c);
      }
      case sf::Event::KeyPressed:
      {
        sf::Keyboard::Key key = e.key.code;
        if (key == sf::Keyboard::Delete || key == sf::Keyboard::Home || key == sf::Keyboard::End ||
          key == sf::Keyboard::Left || key == sf::Keyboard::Right)
        {
          ipInput.TextEntered(key, true);
          portInput.TextEntered(key, true);
          minInput.TextEntered(key, true);
          maxInput.TextEntered(key, true);
        }
      }
      default:
        break;
      }
    }

    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(wnd);
    bool mousePress = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    float dt = deltaTime.restart().asSeconds();
    ia.Update(dt);
    ia.GetIcon(&wnd);

    if (!mousePress && mouseWasPressed && wnd.hasFocus())
    {
      ipInput.OnMousePress(mousePosition);
      portInput.OnMousePress(mousePosition);
      minInput.OnMousePress(mousePosition);
      maxInput.OnMousePress(mousePosition);
      if (connectButton.ContainsMouse(mousePosition))
      {
        if (!coin.IsConnected())
        {
          std::string ip = ipInput.GetString();
          unsigned int port = atoi(portInput.GetString().c_str());
          TO_CONSOLE("Connecting to %s:%u", ip.c_str(), port);
          std::ofstream output;
          output.open("Assets/IpAndPort.txt");
          if (output)
          {
            output << ip << "\n";
            output << port << "\n";;
            output.close();
          }
          connectButtonPressed = true;
        }
        else
        {
          TO_CONSOLE("Disconnecting...");
          coin.Disconnect();
        }
      }
    }
    mouseWasPressed = mousePress;

    ipInput.Update(dt);
    portInput.Update(dt);
    connectButton.Update(mousePosition);
    minInput.Update(dt);
    maxInput.Update(dt);


    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && wnd.hasFocus() && !portInput.IsActive() && !ipInput.IsActive();

    if (!coin.IsFlipping() && spacePressed && !spaceWasPressed)
    {
      std::ofstream minMaxOut;
      minMaxOut.open("Assets/minMaxValue.txt");
      unsigned int _min = 0;
      unsigned int _max = 1;
      if (minMaxOut)
      {
        std::string minVal = minInput.GetString();
        std::string maxVal = maxInput.GetString();

        _min = (unsigned int)atoi(minVal.c_str());
        _max = (unsigned int)atoi(maxVal.c_str());

        minMaxOut << minVal << "\n";
        minMaxOut << maxVal << "\n";
        minMaxOut.close();
      }
      
      flipSound.play();
      coinJustDone = true;
      moveInfo = false;
      moveTimer = 0;
      info.setString("");
      coin.setPosition(Coin::WND_SIZE.x / 2, Coin::WND_SIZE.y / 1.5);
      coin.setScale(1.0f, 1.0f);

      coin.Flip(_min, _max);
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
    minInput.Draw(&wnd);
    maxInput.Draw(&wnd);
    Console::GetInstance()->Draw(&wnd);
    if (!coinJustDone)
      wnd.draw(pressSpace);
    wnd.draw(info);
    wnd.display();

    if (connectButtonPressed)
    {
      coin.Connect(ipInput.GetString(), atoi(portInput.GetString().c_str()));
      if (coin.IsConnected())
        TO_CONSOLE("Connected!");
    }
    if (coin.IsConnected())
      connectButton.setTexture(&connected);
    else
      connectButton.setTexture(&disconnected);

  }

  return 0;
}