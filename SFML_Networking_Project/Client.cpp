#include "Client.h"
#include "Game.h"

Client::Client(sf::IpAddress ip, unsigned short ServerPort, Game& game) :
	mThread(&Client::update, this),
	mWaitingThreadEnd(false),
	mClientTimeout(sf::seconds(2.f)),
	mTimeSinceLastPacket(sf::seconds(0.f))
{
	world = &game;
	ballObject = world->ballObj;
	//std::cout << "client constructor" << std::endl;
	if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done) {
		mConnected = true;
		std::cout << "client connected" << std::endl;
	}
	else
		//mFailedConnectionClock.restart();

		mSocket.setBlocking(false);
	mThread.launch();
}


Client::~Client()
{
	mWaitingThreadEnd = true;
	mThread.wait();
}

void Client::handlePacket(sf::Int32 packetType, sf::Packet& packet)
{
	switch (packetType)
	{
	case packetServer::BallReverse:
	{
		ballObject->toggleDirection();
		std::cout << "client receive ballReverse" << std::endl;
		packet >> ballObject->thetaX >> ballObject->thetaY;

		//receive the ball xyz position and the angle it's travelling at when colliding on the opponent side
		sf::CircleShape* ball = ballObject->getBall();
		float ballCollisionX, ballCollisionY;
		packet >> ballCollisionX >> ballCollisionY >> ballObject->zDepth;
		ball->setPosition(ballCollisionX, ballCollisionY);
		std::cout << "CLRX - x:" << ballCollisionX << " y:" << ballCollisionY << std::endl;

	} break;
	case packetServer::UpdateClientState:
	{
		sf::Vector2f paddlePositionUpdate;
		packet >> paddlePositionUpdate.x >> paddlePositionUpdate.y;
		sf::Vector2f currentPaddlePos(world->getEnemyPaddlePosition());
		world->setEnemyPaddlePosition(paddlePositionUpdate);

		//float scale
	} break;
	case packetServer::StartGame:
	{
		bool isBallIdle;
		packet >> isBallIdle;
		ballObject->isIdle = isBallIdle;

		packet >> ballObject->thetaX >> ballObject->thetaY;

	}


	}
}

void Client::update()
{
	sf::Time dt = sf::Time::Zero;
	while (!mWaitingThreadEnd) {
		dt = world->getTime();

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

			if (world->ballCollide) {
				//send a packet stating the ball has changed direction
				sf::Packet ballReversePacket;
				ballReversePacket << static_cast<sf::Int32>(packetClient::BallReverse);
				std::cout << "client send ballReverse" << std::endl;

				float invertedBallXAngle = -1 * ballObject->thetaX;
				float ballYAngle = ballObject->thetaY;
				ballReversePacket << invertedBallXAngle << ballYAngle;
				//send the ball xyz position and the angle it's travelling at when colliding locally
				sf::Vector2f ballPosition = ballObject->getBall()->getPosition();
				float invertedBallPosX = world->window->getSize().x - ballPosition.x;
				ballReversePacket << invertedBallPosX << ballPosition.y << ballObject->zDepth;
				std::cout << "CLTX - x:" << invertedBallPosX << " y:" << ballPosition.y << std::endl;
				mSocket.send(ballReversePacket);
				//reset flag
				world->ballCollide = false;
			}

			mTimeSinceLastPacket += dt;
		}
	}

}
