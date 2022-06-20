#pragma once
#include <SFML/Graphics.hpp>
#include <deque>

#define CONSOLE_LINE_LENGTH 256

class Console : public sf::RectangleShape
{
private:
  Console() = default;
  ~Console() = default;

public:
  static void Init();
  static Console* GetInstance();
  void Print(const char* file, const char* line, const char* format, ...);
  void Print(const char* format, ...);
  void Clear();
  void Draw(sf::RenderTarget* rt_p);
  void SetLineMemory(unsigned int lineMemLength);
  void SetCharSize(unsigned int charSize);
  std::string CreateTimestamp();
  
private:
  static Console CONSOLE;
  std::deque<std::string> myTexts;
  sf::Font myFont;
  unsigned int myLineMem;
  unsigned int myCharSize;
};

#define TO_CONSOLE_DBG(format, ...) Console::GetInstance()->Print(__FILE__, __LINE__, format, __VA_ARGS__);
#define TO_CONSOLE(format, ...) Console::GetInstance()->Print(format, __VA_ARGS__);