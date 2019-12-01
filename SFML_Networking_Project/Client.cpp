#include "Client.h"
#include "Game.h"

Client::Client(sf::IpAddress ip, unsigned short ServerPort, Game& game):
	mClientTimeout(sf::seconds(2.f)),
	mTimeSinceLastPacket(sf::seconds(0.f))//,
	//mThread(&Client::readNetPacket,this)
	/*,
	world(&game)*/
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
			// Send message to all clients
		//case packetServer::BroadcastMessage:
		//{
		//	std::string message;
		//	packet >> message;
		//	mBroadcasts.push_back(message);

		//	// Just added first message, display immediately
		//	if (mBroadcasts.size() == 1)
		//	{
		//		mBroadcastText.setString(mBroadcasts.front());
		//		centerOrigin(mBroadcastText);
		//		mBroadcastElapsedTime = sf::Time::Zero;
		//	}
		//} break;

		// Sent by the server to order to spawn player 1 airplane on connect
		//case packetServer::SpawnSelf:
		//{
		//	sf::Int32 aircraftIdentifier;
		//	sf::Vector2f aircraftPosition;
		//	packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

		//	Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		//	aircraft->setPosition(aircraftPosition);

		//	mPlayers[aircraftIdentifier].reset(new Player(&mSocket, aircraftIdentifier, getContext().keys1));
		//	mLocalPlayerIdentifiers.push_back(aircraftIdentifier);

		//	mGameStarted = true;
		//} break;

		// 
		//case packetServer::PlayerConnect:
		//{
		//	//sf::Int32 aircraftIdentifier;
		//	sf::Vector2f aircraftPosition;
		//	packet >> aircraftPosition.x >> aircraftPosition.y;

		//	Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		//	aircraft->setPosition(aircraftPosition);

		//	mPlayers[aircraftIdentifier].reset(new Player(&mSocket, aircraftIdentifier, nullptr));
		//} break;

		// 
		//case packetServer::PlayerDisconnect:
		//{
		//	sf::Int32 aircraftIdentifier;
		//	packet >> aircraftIdentifier;

		//	mWorld.removeAircraft(aircraftIdentifier);
		//	mPlayers.erase(aircraftIdentifier);
		//} break;

		// 
		//case packetServer::InitialState:
		//{
		//	//sf::Int32 aircraftCount;
		//	//float worldHeight, currentScroll;
		//	//packet >> worldHeight >> currentScroll;

		//	mWorld.setWorldHeight(worldHeight);
		//	mWorld.setCurrentBattleFieldPosition(currentScroll);

		//	packet >> aircraftCount;
		//	for (sf::Int32 i = 0; i < aircraftCount; ++i)
		//	{
		//		sf::Int32 aircraftIdentifier;
		//		sf::Int32 hitpoints;
		//		sf::Int32 missileAmmo;
		//		sf::Vector2f aircraftPosition;
		//		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y >> hitpoints >> missileAmmo;

		//		Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		//		aircraft->setPosition(aircraftPosition);
		//		aircraft->setHitpoints(hitpoints);
		//		aircraft->setMissileAmmo(missileAmmo);

		//		mPlayers[aircraftIdentifier].reset(new Player(&mSocket, aircraftIdentifier, nullptr));
		//	}
		//} break;

		//
		//case packetServer::AcceptCoopPartner:
		//{
		//	sf::Int32 aircraftIdentifier;
		//	packet >> aircraftIdentifier;

		//	mWorld.addAircraft(aircraftIdentifier);
		//	mPlayers[aircraftIdentifier].reset(new Player(&mSocket, aircraftIdentifier, getContext().keys2));
		//	mLocalPlayerIdentifiers.push_back(aircraftIdentifier);
		//} break;

		// Player event (like missile fired) occurs
		//case packetServer::PlayerEvent:
		//{
		//	sf::Int32 aircraftIdentifier;
		//	sf::Int32 action;
		//	packet >> aircraftIdentifier >> action;

		//	auto itr = mPlayers.find(aircraftIdentifier);
		//	if (itr != mPlayers.end())
		//		itr->second->handleNetworkEvent(static_cast<Player::Action>(action), mWorld.getCommandQueue());
		//} break;

		// Player's movement or fire keyboard state changes
		//case packetServer::PlayerRealtimeChange:
		//{
		//	sf::Int32 aircraftIdentifier;
		//	sf::Int32 action;
		//	bool actionEnabled;
		//	packet >> aircraftIdentifier >> action >> actionEnabled;

		//	auto itr = mPlayers.find(aircraftIdentifier);
		//	if (itr != mPlayers.end())
		//		itr->second->handleNetworkRealtimeChange(static_cast<Player::Action>(action), actionEnabled);
		//} break;

		// New enemy to be created
		//case packetServer::SpawnEnemy:
		//{
		//	float height;
		//	sf::Int32 type;
		//	float relativeX;
		//	packet >> type >> height >> relativeX;

		//	mWorld.addEnemy(static_cast<Aircraft::Type>(type), relativeX, height);
		//	mWorld.sortEnemies();
		//} break;

		// Mission successfully completed
		//case packetServer::MissionSuccess:
		//{
		//	requestStackPush(States::MissionSuccess);
		//} break;

		// Pickup created
		//case packetServer::SpawnPickup:
		//{
		//	sf::Int32 type;
		//	sf::Vector2f position;
		//	packet >> type >> position.x >> position.y;

		//	mWorld.createPickup(position, static_cast<Pickup::Type>(type));
		//} break;

		//
		case packetServer::UpdateClientState:
		{
			//float currentWorldPosition;
			//sf::Int32 aircraftCount;
			//packet >> currentWorldPosition >> aircraftCount;

			//float currentViewPosition = mWorld.getViewBounds().top + mWorld.getViewBounds().height;

			// Set the world's scroll compensation according to whether the view is behind or too advanced
			//mWorld.setWorldScrollCompensation(currentViewPosition / currentWorldPosition);

			//for (sf::Int32 i = 0; i < aircraftCount; ++i)
			//{
				sf::Vector2f paddlePositionUpdate;

				//sf::Int32 aircraftIdentifier;
				packet >> paddlePositionUpdate.x >> paddlePositionUpdate.y;
				sf::Vector2f currentPaddlePos(world->getMyPaddlePositon());
				sf::Vector2f interpolatedPosition = currentPaddlePos + (paddlePositionUpdate - currentPaddlePos) * 0.1f;
				
				world->setEnemyPaddlePosition(interpolatedPosition);
				//std::cout << "Recieved position" << std::endl;
				//std::cout << paddlePosition.x << " " << paddlePosition.y << std::endl;

				/*world.setEnemyPaddlePosition(paddlePosition);*/
				//Aircraft* aircraft = mWorld.getAircraft(aircraftIdentifier);
				//bool isLocalPlane = std::find(mLocalPlayerIdentifiers.begin(), mLocalPlayerIdentifiers.end(), aircraftIdentifier) != mLocalPlayerIdentifiers.end();
				//if (aircraft && !isLocalPlane)
				//{
				//	sf::Vector2f interpolatedPosition = aircraft->getPosition() + (aircraftPosition - aircraft->getPosition()) * 0.1f;
				//	aircraft->setPosition(interpolatedPosition);
				//}
			//}
		} break;
	}
}

bool Client::update(sf::Time dt)
{
	// Connected to server: Handle all the network logic
	if (mConnected)
	{
		//mWorld.update(dt);

		// Remove players whose aircrafts were destroyed
		//bool foundLocalPlane = false;
		//for (auto itr = mPlayers.begin(); itr != mPlayers.end(); )
		//{
			// Check if there are no more local planes for remote clients
			//if (std::find(mLocalPlayerIdentifiers.begin(), mLocalPlayerIdentifiers.end(), itr->first) != mLocalPlayerIdentifiers.end())
			//{
			//	foundLocalPlane = true;
			//}

			//if (!mWorld.getAircraft(itr->first))
			//{
			//	itr = mPlayers.erase(itr);

			//	 No more players left: Mission failed
			//	if (mPlayers.empty())
					//requestStackPush(States::GameOver);
			//}
			//else
			//{
				//++itr;
			//}
		//}

		//if (!foundLocalPlane && mGameStarted)
		//{
		//	requestStackPush(States::GameOver);
		//}

		// Only handle the realtime input if the window has focus and the game is unpaused
		//if (mActiveState && mHasFocus)
		//{
		//	CommandQueue& commands = mWorld.getCommandQueue();
		//	FOREACH(auto& pair, mPlayers)
		//		pair.second->handleRealtimeInput(commands);
		//}

		// Always handle the network input
		//CommandQueue& commands = mWorld.getCommandQueue();
		//FOREACH(auto& pair, mPlayers)
		//	pair.second->handleRealtimeNetworkInput(commands);

		// Handle messages from server that may have arrived
		sf::Packet packet;
		if (mSocket.receive(packet) == sf::Socket::Done)
		{
			mTimeSinceLastPacket = sf::seconds(0.f);
			sf::Int32 packetType;
			packet >> packetType;
			handlePacket(packetType, packet);
		}
		//else
		//{
		//	// Check for timeout with the server
		//	if (mTimeSinceLastPacket > mClientTimeout)
		//	{
		//		mConnected = false;

		//		//mFailedConnectionText.setString("Lost connection to server");
		//		//centerOrigin(mFailedConnectionText);

		//		//mFailedConnectionClock.restart();
		//	}
		//}

		//updateBroadcastMessage(dt);

		// Time counter for blinking 2nd player text
		//mPlayerInvitationTime += dt;
		//if (mPlayerInvitationTime > sf::seconds(1.f))
			//mPlayerInvitationTime = sf::Time::Zero;

		// Events occurring in the game
		//GameActions::Action gameAction;
		//while (mWorld.pollGameAction(gameAction))
		//{
		//	sf::Packet packet;
		//	packet << static_cast<sf::Int32>(Client::GameEvent);
		//	packet << static_cast<sf::Int32>(gameAction.type);
		//	packet << gameAction.position.x;
		//	packet << gameAction.position.y;

		//	mSocket.send(packet);
		//}

		// Regular position updates
		if (mTickClock.getElapsedTime() > sf::seconds(1.f / 20.f))
		{
			sf::Packet positionUpdatePacket;
			positionUpdatePacket << static_cast<sf::Int32>(packetClient::PositionUpdate);
			//positionUpdatePacket << static_cast<sf::Int32>(mLocalPlayerIdentifiers.size());

			//FOREACH(sf::Int32 identifier, mLocalPlayerIdentifiers)
			//{
				//if (Aircraft* aircraft = mWorld.getAircraft(identifier))
			sf::Vector2f paddlePos(world->getMyPaddlePositon());
					positionUpdatePacket << paddlePos.x << paddlePos.y;
			//}
					//std::cout << "sending position" << std::endl;
					//std::cout << serverPaddle.position.x << " " <<serverPaddle.position.y << std::endl;

			mSocket.send(positionUpdatePacket);
			mTickClock.restart();
		}

		mTimeSinceLastPacket += dt;
	}

	// Failed to connect and waited for more than 5 seconds: Back to menu
	//else if (mFailedConnectionClock.getElapsedTime() >= sf::seconds(5.f))
	//{
	//	requestStateClear();
	//	requestStackPush(States::Menu);
	//}

	return true;
}
