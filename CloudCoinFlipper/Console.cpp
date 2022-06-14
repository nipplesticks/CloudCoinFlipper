#include "Console.h"
#include <cstdarg>
#include <iostream>
#include <strsafe.h>
#include <windows.h>

Console Console::CONSOLE;

void Console::Init()
{
  GetInstance()->myFont.loadFromFile("Assets/BASKVILL.TTF");
  GetInstance()->myLineMem = 1024;
  GetInstance()->setFillColor(sf::Color(0, 0, 0, 128));
  GetInstance()->setSize(sf::Vector2f(300, 200));
  GetInstance()->myCharSize = 16;
}

Console* Console::GetInstance()
{
  return &CONSOLE;
}

void Console::Print(const char* file, const char* line, const char* format, ...)
{
  va_list argList = nullptr;
  va_start(argList, format);
  char logMessage[CONSOLE_LINE_LENGTH];
  memset(logMessage, 0, sizeof(char) * CONSOLE_LINE_LENGTH);
  _vsnprintf_s(logMessage, CONSOLE_LINE_LENGTH, format, argList);
  va_end(argList);

  char finalMessage[CONSOLE_LINE_LENGTH];
  memset(finalMessage, 0, sizeof(char) * CONSOLE_LINE_LENGTH);

  auto printer = [](char message[CONSOLE_LINE_LENGTH], const char* finalFormat, ...) {
    va_list argList = nullptr;
    va_start(argList, finalFormat);
    _vsnprintf_s(message, CONSOLE_LINE_LENGTH, CONSOLE_LINE_LENGTH, finalFormat, argList);
    va_end(argList);
  };
  printer(finalMessage, "%s (%d): %s", file, line, logMessage);

  std::string msg(finalMessage);
  myTexts.push_back(msg);
  while (myTexts.size() > myLineMem)
    myTexts.pop_front();
}

void Console::Print(const char* format, ...)
{
  va_list argList = nullptr;
  va_start(argList, format);
  char logMessage[CONSOLE_LINE_LENGTH];
  memset(logMessage, 0, sizeof(char) * CONSOLE_LINE_LENGTH);
  _vsnprintf_s(logMessage, CONSOLE_LINE_LENGTH, format, argList);
  va_end(argList);

  std::string line(logMessage);
  myTexts.push_back(line);
  while (myTexts.size() > myLineMem)
    myTexts.pop_front();
}

void Console::Clear()
{
  myTexts.clear();
}

void Console::Draw(sf::RenderTarget* rt_p)
{
  sf::Vector2f startPoint = getPosition() + sf::Vector2f(5, getSize().y);
  float maxW = getPosition().x + getSize().x - 5.f;
  rt_p->draw(*this);
  for (int i = myTexts.size() - 1; i >= 0; i--)
  {
    std::string str = myTexts[i];
    sf::Text line(str, myFont, myCharSize);

    if (line.getGlobalBounds().left + line.getGlobalBounds().width > getPosition().x + getSize().x)
    {
      size_t n = str.length();

      while (n-- > 0 && n != (size_t)-1)
      {
        float cW = line.findCharacterPos(n).x;
        if (cW < maxW)
        {
          std::string a = str.substr(0, n);
          std::string b = str.substr(n);
          myTexts.erase(myTexts.begin() + i);
          myTexts.insert(myTexts.begin() + i, a);
          myTexts.insert(myTexts.begin() + i + 1, b);
          i++;
          break;
        }
      }
    }
    else
    {
      line.setPosition(startPoint -= sf::Vector2f(0, 5 + line.getGlobalBounds().height));
      if (line.getPosition().y < getPosition().y)
        break;
      rt_p->draw(line);
    }
  }
}

void Console::SetLineMemory(unsigned int lineMemLength)
{
  myLineMem = lineMemLength;
}

void Console::SetCharSize(unsigned int charSize)
{
  myCharSize = charSize;
}
