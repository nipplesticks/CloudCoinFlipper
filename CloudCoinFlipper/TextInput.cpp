#include "Console.h"
#include "TextInput.h"

const float TextInput::BLINK_TIME = 0.5f;

TextInput::TextInput() : sf::RectangleShape()
{
  myFont.loadFromFile("Assets/BASKVILL.TTF");
  myText.setFont(myFont);
  myTextBoxName.setFont(myFont);
  myTextBoxName.setOutlineThickness(1);
  myCursor.setFillColor(sf::Color::White);
  setFillColor(sf::Color(0, 0, 0, 200));
  setOutlineThickness(-1.0f);
  setOutlineColor(sf::Color::White);
  myIsActive = false;
}

void TextInput::OnMousePress(const sf::Vector2f& mousePosition)
{
  myIsActive = false;
  if (getGlobalBounds().contains(mousePosition))
  {
    myIsActive = true;

    float mx = mousePosition.x;

    std::string txt = myText.getString();
    myCursorIndex = txt.length();
    myCursor.setPosition(myText.findCharacterPos(myCursorIndex).x, myCursor.getPosition().y);
    for (unsigned int i = 0; i < txt.length(); i++)
    {
      float tx = myText.findCharacterPos(i).x;
      if (tx > mx)
      {
        myCursorIndex = i;
        myCursor.setPosition(tx, myCursor.getPosition().y);
        break;
      }
    }
  }
}

void TextInput::Update(float dt)
{
  myCursor.setSize(sf::Vector2f(5, getSize().y - (getSize().y - myText.getCharacterSize())));
  myText.setPosition(getPosition() + sf::Vector2f(10, 5));
  myTextBoxName.setPosition(getPosition() + sf::Vector2f(10, 5));
  myTextBoxName.setOrigin(sf::Vector2f(myTextBoxName.getGlobalBounds().width + 20, 0));
  myCursor.setPosition(myCursor.getPosition().x, getPosition().y + 10);

  myTimer += dt;
  while (myTimer > BLINK_TIME + myCursorVisible * 0.5f)
  {
    myTimer -= BLINK_TIME + myCursorVisible * 0.5f;
    myCursorVisible = !myCursorVisible;
  }
}

void TextInput::TextEntered(sf::Uint32 c, bool specialKeyCommand)
{
  if (myIsActive)
  {
    std::string str = myText.getString();
    if (!specialKeyCommand)
    {
      //TO_CONSOLE(c);
      if (c >= 30 && c <= 256 && str.length() < myMaxStringLength)
      {
        str.insert(str.begin() + myCursorIndex++, c);
      }
      else if (c == 8 && !str.empty())
      {
        if (myCursorIndex > 0)
        {
          myCursorIndex--;
          str.erase(str.begin() + myCursorIndex);
        }
      }
    }
    else
    {
      sf::Keyboard::Key key = (sf::Keyboard::Key)c;

      if (key == sf::Keyboard::Delete)
      {
        if (myCursorIndex != str.length())
        {
          str.erase(str.begin() + myCursorIndex);
        }
      }
      else if (key == sf::Keyboard::Home)
      {
        myCursorIndex = 0;
      }
      else if (key == sf::Keyboard::End)
      {
        myCursorIndex = str.length();
      }
      else if (key == sf::Keyboard::Right)
      {
        myCursorIndex++;
      }
      else if (key == sf::Keyboard::Left)
      {
        if (myCursorIndex > 0)
        {
          myCursorIndex--;
        }
      }
    }

    if (myCursorIndex > str.length())
      myCursorIndex = str.length();
    myText.setString(str);
    myCursor.setPosition(myText.findCharacterPos(myCursorIndex).x, myCursor.getPosition().y);
  }
}

void TextInput::SetString(const std::string& string)
{
  myText.setString(string);
}

void TextInput::SetTextBoxName(const std::string& string)
{
  myTextBoxName.setString(string);
}

void TextInput::Draw(sf::RenderTarget* target_p)
{
  target_p->draw(*this);
  target_p->draw(myText);
  target_p->draw(myTextBoxName);
  if (myIsActive && myCursorVisible)
    target_p->draw(myCursor);
}
