#pragma once
#include <SFML/Graphics.hpp>
class TextInput : public sf::RectangleShape
{
public:
  TextInput();
  ~TextInput() = default;

  void OnMousePress(const sf::Vector2f& mousePosition);

  void Update(float dt);

  void TextEntered(sf::Uint32 c, bool specialKeyCommand = false);

  void SetString(const std::string& string);

  void SetTextBoxName(const std::string& string);

  void Draw(sf::RenderTarget* target_p);

  bool IsActive() const { return myIsActive; }

  std::string GetString() { return myText.getString(); }

  void SetStrMaxLength(unsigned int maxLength) { myMaxStringLength = maxLength; }

private:
  static const float BLINK_TIME;
  sf::Font myFont;
  sf::Text myText;
  sf::Text myTextBoxName;
  sf::RectangleShape myCursor;
  bool myCursorVisible = true;
  bool myIsActive;
  unsigned int myCursorIndex = 0;
  float myTimer = 0.0f;
  unsigned int myMaxStringLength = -1;
};