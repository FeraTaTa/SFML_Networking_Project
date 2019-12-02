#include "Client.h"
#include "Game.h"

Client::Client(sf::IpAddress ip, unsigned short ServerPort, Game& game):
	mClientTimeout(sf::seconds(2.f)),
	mTimeSinceLastPacket(sf::seconds(0.f))
{
	world = &game;
	std::cout << "client constructor" << std::endl;
	if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done) {
		mConnected = true;
		std::cout << "client connected" << std::endl;
	}
	else
		//mFailedConnectionClock.restart();

	mSocket.setBlocking(false);
}


Client::~Client()
{
}

void Client::handlePacket(sf::Int32 packetType, sf::Packet& packet)
{
	switch (packetType)
	{
		case packetServer::UpdateClientState:
		{
				sf::Vector2f paddlePositionUpdate;
				packet >> paddlePositionUpdate.x >> paddlePositionUpdate.y;
				sf::Vector2f currentPaddlePos(world->getMyPaddlePositon());
				sf::Vector2f interpolatedPosition = currentPaddlePos + (paddlePositionUpdate - currentPaddlePos) * 0.1f;
				
				world->setEnemyPaddlePosition(interpolatedPosition);
				//std::cout << "Recieved position" << std::endl;
				//std::cout << paddlePosition.x << " " << paddlePosition.y << std::endl;
		} break;
	}
}

bool Client::update(sf::Time dt)
{
	// Connected to server: Handle all the network logic
	if (mConnected)
	{
		// Handle messages from server that may have arrived
		sf::Packet packet;
		if (mSocket.receive(packet) == sf::Socket::Done)
		{
			mTimeSinceLastPacket = sf::seconds(0.f);
			sf::Int32 packetType;
			packet >> packetType;
			handlePacket(packetType, packet);
		}
		else
		{
			// Check for timeout with the server
			if (mTimeSinceLastPacket > mClientTimeout)
			{
				std::cout << "Client timeout" << std::endl;
				mConnected = false;
			}
		}

		// Regular position updates
		if (mTickClock.getElapsedTime() > sf::seconds(1.f / 20.f))
		{
			sf::Packet positionUpdatePacket;
			positionUpdatePacket << static_cast<sf::Int32>(packetClient::PositionUpdate);
			sf::Vector2f paddlePos(world->getMyPaddlePositon());
			positionUpdatePacket << paddlePos.x << paddlePos.y;
			//std::cout << "sending position" << std::endl;
			//std::cout << serverPaddle.position.x << " " <<serverPaddle.position.y << std::endl;

			mSocket.send(positionUpdatePacket);
			mTickClock.restart();
		}

		mTimeSinceLastPacket += dt;
	}

	return true;
}
