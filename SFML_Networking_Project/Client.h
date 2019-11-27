#pragma once
#include "GameState.h"

class Game; // Say foo exists without defining it.

//class Game {
//public:
//	Game* getGame();
//protected:
//	Game* gameWorld;
//};

class Client
{
private:
	bool mConnected;
	sf::TcpSocket				mSocket;
	sf::Time mTimeSinceLastPacket;
	sf::Time mClientTimeout;
	sf::Clock					mTickClock;

	Game* world;

	PaddleInfo clientPaddle;
	PaddleInfo serverPaddle;

public:
	Client(sf::IpAddress ip, unsigned short ServerPort, Game& game);
	virtual ~Client();
	
	void handlePacket(sf::Int32 packetType, sf::Packet& packet);
	bool update(sf::Time dt);
};
