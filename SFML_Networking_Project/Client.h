#pragma once
#include "GameState.h"

class Game;
class Ball;

class Client
{
private:
	bool mConnected;
	sf::TcpSocket mSocket;
	sf::Time mTimeSinceLastPacket;
	sf::Time mClientTimeout;
	sf::Clock mTickClock;
	sf::Thread mThread;
	bool mWaitingThreadEnd;

	Game* world;
	Ball* ballObject;

	PaddleInfo clientPaddle;
	PaddleInfo serverPaddle;

public:
	Client(sf::IpAddress ip, unsigned short ServerPort, Game& game);
	virtual ~Client();

	void handlePacket(sf::Int32 packetType, sf::Packet& packet);
	void update();
};
