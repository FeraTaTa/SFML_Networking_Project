#include "Server.h"
#include "Game.h"


Server::Server(sf::IpAddress ip, unsigned short ServerPort, sf::Vector2u battlefieldSize, Game& game) :
	mThread(&Server::executionThread, this), 
	mMaxConnectedPlayers(1), 
	mPeer(), 
	mConnectedPlayers(0), 
	mWaitingThreadEnd(false), 
	mListeningState(false), 
	mClientTimeoutTime(sf::seconds(3.f)), 
	mBattleFieldRect(0.f, 0.f, battlefieldSize.x, battlefieldSize.y)
{
	world = &game;
	ballObject = world->ballObj;
	mListenerSocket.setBlocking(false);
	mPeer.reset(new RemotePeer());
	mThread.launch();
}

Server::~Server()
{
	mWaitingThreadEnd = true;
	mThread.wait();
}

void Server::setListening(bool enable)
{
	// Check if it isn't already listening
	if (enable)
	{
		if (!mListeningState) {
			mListeningState = (mListenerSocket.listen(ServerPort) == sf::TcpListener::Done);
			std::cout << "listener socket ready" << std::boolalpha << mListeningState << std::endl;
		}
	}
	else
	{
		mListenerSocket.close();
		mListeningState = false;
	}
}

void Server::executionThread()
{
	setListening(true);

	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd)
	{
		handleIncomingPacket(*mPeer);
		handleIncomingConnections();

		stepTime += stepClock.getElapsedTime();
		stepClock.restart();

		tickTime += tickClock.getElapsedTime();
		tickClock.restart();

		// Fixed update step
		while (stepTime >= stepInterval)
		{
			//step();
			stepTime -= stepInterval;
		}

		// Fixed tick step
		while (tickTime >= tickInterval)
		{
			tick();
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(1));
	}
}

void Server::tick()
{
	if (world->gameStart) {
		//send a packet stating the ball has started moving with its initial angle
		sf::Packet gameStartPacket;
		gameStartPacket << static_cast<sf::Int32>(packetServer::StartGame);

		bool isIdle = ballObject->isIdle;
		float invertedBallXAngle = -1 * ballObject->thetaX;
		float ballYAngle = ballObject->thetaY;

		gameStartPacket << isIdle << invertedBallXAngle << ballYAngle;
		sendToClient(gameStartPacket);

		world->gameStart = false;
	}

	//if ball collide flag set then change direction
	if (world->ballCollide) {
		//send a packet stating the ball has changed direction
		sf::Packet ballReversePacket;
		ballReversePacket << static_cast<sf::Int32>(packetServer::BallReverse);

		float invertedBallXAngle = -1 * ballObject->thetaX;
		float ballYAngle = ballObject->thetaY;
		ballReversePacket << invertedBallXAngle << ballYAngle;

		//send the ball xyz position and the angle it's travelling at when colliding locally
		sf::Vector2f ballPosition = ballObject->getBall()->getPosition();
		float invertedBallPosX = world->window->getSize().x - ballPosition.x;
		ballReversePacket << invertedBallPosX << ballPosition.y << ballObject->zDepth;
		sendToClient(ballReversePacket);
		std::cout << std::endl << "server send ballReverse" << std::endl;
		std::cout << "SVTX - x:" << invertedBallPosX << " y:" << ballPosition.y << " z:" << ballObject->zDepth << std::endl;
		std::cout << "SVTX - thetaX:" << invertedBallXAngle << " thetaY:" << ballYAngle << std::endl;
		//reset flag
		world->ballCollide = false;
	}

	//Send data to client every time tick
	if (mConnectedPlayers != 0) {
		sf::Packet serverInfoPacket;
		//get the current position of server paddle (mypaddle)
		sf::Vector2f myPaddlePosition(world->getMyPaddlePositon());
		//create a packet with the correct type and info
		serverInfoPacket << packetServer::UpdateClientState << myPaddlePosition.x << myPaddlePosition.y;
		//send packet
		sendToClient(serverInfoPacket);
	}

}

sf::Time Server::now() const
{
	return mClock.getElapsedTime();
}

void Server::handleIncomingPacket(RemotePeer& receivingPeer)
{
	bool detectedTimeout = false;

	if (mPeer->ready)
	{
		sf::Packet packet;
		while (mPeer->socket.receive(packet) == sf::Socket::Done)
		{
			// Interpret packet and react to it
			sf::Int32 packetType;
			packet >> packetType;

			switch (packetType)
			{
				case packetClient::Quit:
				{
					std::cout << "quit" << std::endl;
					receivingPeer.timedOut = true;
					detectedTimeout = true;
					break;
				} 
				case packetClient::PositionUpdate:
				{
					sf::Vector2f paddlePositionUpdate;
					packet >> paddlePositionUpdate.x >> paddlePositionUpdate.y;
					world->setEnemyPaddlePosition(paddlePositionUpdate);
					break;
				}
				case packetClient::BallReverse:
				{
					ballObject->toggleDirection();
					std::cout << std::endl << "server receive ballReverse" << std::endl;
					packet >> ballObject->thetaX >> ballObject->thetaY;
					//receive the ball xyz position and the angle it's travelling at when colliding on the opponent side
					sf::CircleShape* ball = ballObject->getBall();
					float ballCollisionX, ballCollisionY;
					packet >> ballCollisionX >> ballCollisionY >> ballObject->zDepth;
					ball->setPosition(ballCollisionX, ballCollisionY);
					std::cout << "SVRX - x:" << ballCollisionX << " y:" << ballCollisionY << " z:" << ballObject->zDepth << std::endl;
					std::cout << "SVRX - thetaX:" << ballObject->thetaX << " thetaY:" << ballObject->thetaY << std::endl;
					break;
				} 
			}
			// Packet was indeed received, update the ping timer
			mPeer->lastPacketTime = now();
			packet.clear();
		}

		if (now() >= mPeer->lastPacketTime + mClientTimeoutTime)
		{
			std::cout << "time out" << std::endl;
			mPeer->timedOut = true;
			mPeer->ready = false;
			detectedTimeout = true;
		}
	}

	if (detectedTimeout)
		handleDisconnections();
}

void Server::handleIncomingConnections()
{
	if (!mListeningState)
		return;

	if (mListenerSocket.accept(mPeer->socket) == sf::TcpListener::Done)
	{
		clientPaddle.position = sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);

		sf::Packet packet;
		packet << clientPaddle.position.x;
		packet << clientPaddle.position.y;

		mPeer->socket.send(packet);
		mPeer->ready = true;
		mPeer->lastPacketTime = now(); // prevent initial timeouts
		std::cout << "Client connected" << std::endl;
		mConnectedPlayers++;
		if (mConnectedPlayers >= mMaxConnectedPlayers) {
			setListening(false);
		}
	}
}

void Server::handleDisconnections()
{
}

void Server::sendToClient(sf::Packet& packet)
{
	if (mPeer->ready) {
		mPeer->socket.send(packet);
	}
}

Server::RemotePeer::RemotePeer(): 
	ready(false), 
	timedOut(false)
{
	socket.setBlocking(false);
}