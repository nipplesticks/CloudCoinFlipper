#pragma once
#include <SFML/Network.hpp>

class CoinNetwork
{
public:

private:
  sf::TcpSocket mySocket;
  sf::TcpListener myListener;
  sf::TcpSocket myClient;
  sf::IpAddress myRecipient;
  sf::IpAddress mySender;
};