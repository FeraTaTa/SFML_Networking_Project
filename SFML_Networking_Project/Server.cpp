#include "Server.h"
#include "Game.h"


Server::Server(sf::IpAddress ip, unsigned short ServerPort, sf::Vector2u battlefieldSize, Game& game):
	mThread(&Server::executionThread, this)
		, mMaxConnectedPlayers(1)
		, mPeer()
		, mConnectedPlayers(0)
		//, mAircraftIdentifierCounter(1)
		, mWaitingThreadEnd(false)
		//, mAircraftCount(0)
		, mListeningState(false)
		, mClientTimeoutTime(sf::seconds(3.f))
		//, mWorldHeight(5000.f)
		, mBattleFieldRect(0.f, 0.f, battlefieldSize.x, battlefieldSize.y)
		//, mBattleFieldScrollSpeed(-50.f)
		//, mTimeForNextSpawn(sf::seconds(5.f))
		//, mLastSpawnTime(sf::Time::Zero)
	{
		world = &game;
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
			std::cout << "listen setup" << std::boolalpha << mListeningState << std::endl;
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
			//mBattleFieldRect.top += mBattleFieldScrollSpeed * stepInterval.asSeconds();
			stepTime -= stepInterval;
		}

		// Fixed tick step
		while (tickTime >= tickInterval)
		{
			tick();
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void Server::tick()
{
	if (mConnectedPlayers != 0) {
		std::cout << mConnected << std::endl;
		//std::cout << "inside tick" << std::endl;
		sf::Packet hostPaddlePosition;
		//get the current position of server paddle (mypaddle)
		sf::Vector2f myPaddlePosition(world->getMyPaddlePositon());
		//create a packet with the correct type and info
		hostPaddlePosition << packetServer::UpdateClientState << myPaddlePosition.x << myPaddlePosition.y;
		std::cout << myPaddlePosition.x<< " " << myPaddlePosition.y << std::endl;
		//send packet
		sendToClient(hostPaddlePosition);
		//std::cout << "sent packet position in tick" << std::endl;
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
				} break;

				/*case Client::PlayerEvent:
				{
					sf::Int32 aircraftIdentifier;
					sf::Int32 action;
					packet >> aircraftIdentifier >> action;

					notifyPlayerEvent(aircraftIdentifier, action);
				} break;

				case Client::PlayerRealtimeChange:
				{
					sf::Int32 aircraftIdentifier;
					sf::Int32 action;
					bool actionEnabled;
					packet >> aircraftIdentifier >> action >> actionEnabled;
					mAircraftInfo[aircraftIdentifier].realtimeActions[action] = actionEnabled;
					notifyPlayerRealtimeChange(aircraftIdentifier, action, actionEnabled);
				} break;*/

				case packetClient::PositionUpdate:
				{
					//sf::Int32 numAircrafts;
					//packet >> numAircrafts;

					//for (sf::Int32 i = 0; i < numAircrafts; ++i)
					//{
						//sf::Int32 aircraftIdentifier;
						//sf::Int32 aircraftHitpoints;
						//sf::Int32 missileAmmo;
					sf::Vector2f paddlePosition;
					packet >> paddlePosition.x >> paddlePosition.y;
/*
					std::cout << "read inc packet from client" << std::endl;
					std::cout << serverPaddle.position.x << " " << serverPaddle.position.y << std::endl;*/

					//mAircraftInfo[aircraftIdentifier].position = aircraftPosition;
					//mAircraftInfo[aircraftIdentifier].hitpoints = aircraftHitpoints;
					//mAircraftInfo[aircraftIdentifier].missileAmmo = missileAmmo;
				//}
				} break;

				//case Client::GameEvent:
				//{
				//	sf::Int32 action;
				//	float x;
				//	float y;

				//	packet >> action;
				//	packet >> x;
				//	packet >> y;

				//	// Enemy explodes: With certain probability, drop pickup
				//	// To avoid multiple messages spawning multiple pickups, only listen to first peer (host)
				//	if (action == GameActions::EnemyExplode && randomInt(3) == 0 && &receivingPeer == mPeers[0].get())
				//	{
				//		sf::Packet packet;
				//		packet << static_cast<sf::Int32>(Server::SpawnPickup);
				//		packet << static_cast<sf::Int32>(randomInt(Pickup::TypeCount));
				//		packet << x;
				//		packet << y;

				//		sendToAll(packet);
				//	}
				//}
				}

				// Packet was indeed received, update the ping timer
				mPeer->lastPacketTime = now();
				packet.clear();
			}

			if (now() >= mPeer->lastPacketTime + mClientTimeoutTime)
			{
				std::cout << "time out" << std::endl;
				mPeer->timedOut = true;
				detectedTimeout = true;
			}
		}

	if (detectedTimeout)
		handleDisconnections();
}

void Server::handleIncomingConnections()
{
		//std::cout << "enter handle  inc conns" << std::endl;
	if (!mListeningState)
		return;

	if (mListenerSocket.accept(mPeer->socket) == sf::TcpListener::Done)
	{
		//std::cout << "socket accept" << std::endl;
		// order the new client to spawn its own plane ( player 1 )
		clientPaddle.position = sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);
		//mAircraftInfo[mAircraftIdentifierCounter].hitpoints = 100;
		//mAircraftInfo[mAircraftIdentifierCounter].missileAmmo = 2;

		sf::Packet packet;
		//packet << static_cast<sf::Int32>(Server::SpawnSelf);
		//packet << mAircraftIdentifierCounter;
		packet << clientPaddle.position.x;
		packet << clientPaddle.position.y;

		//mPeer-> aircraftIdentifiers.push_back(mAircraftIdentifierCounter);

		//broadcastMessage("New player!");
		//informWorldState(mPeer[mConnectedPlayers]->socket);
		//notifyPlayerSpawn(mAircraftIdentifierCounter++);

		mPeer->socket.send(packet);
		mPeer->ready = true;
		mPeer->lastPacketTime = now(); // prevent initial timeouts
		//mAircraftCount++;
		mConnectedPlayers++;
		if (mConnectedPlayers >= mMaxConnectedPlayers)
			setListening(false);
		//else // Add a new waiting peer
			//mPeer.push_back(PeerPtr(new RemotePeer()));
	}
}

void Server::handleDisconnections()
{
}

void Server::sendToClient(sf::Packet& packet)
{
	if (mPeer->ready) {
		mPeer->socket.send(packet);
		//std::cout << "packet sent" << std::endl;
	}
}

Server::RemotePeer::RemotePeer()
	: ready(false)
	, timedOut(false)
{
	socket.setBlocking(false);
}