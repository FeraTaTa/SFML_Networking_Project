#pragma once
#include "GameState.h"

class Game;
class Ball;

class Server
{
private:


	sf::TcpSocket		mSocket;
	sf::TcpListener		mListenerSocket;
	sf::Thread			mThread;
	sf::Clock			mClock;
	bool				mListeningState;
	sf::FloatRect						mBattleFieldRect;
	std::size_t mMaxConnectedPlayers;
	std::size_t							mConnectedPlayers;
	bool				mConnected;
	bool				mWaitingThreadEnd;
	sf::Time							mClientTimeoutTime;

	Game* world;
	Ball* ballObject;

	// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
	struct RemotePeer
	{
		RemotePeer();

		sf::TcpSocket			socket;
		sf::Time				lastPacketTime;
		bool					ready;
		bool					timedOut;
	};
	typedef std::unique_ptr<RemotePeer>	PeerPtr;
	PeerPtr	mPeer;

	PaddleInfo clientPaddle;
	PaddleInfo serverPaddle;

public:
	Server(sf::IpAddress ip, unsigned short ServerPort, sf::Vector2u battlefieldSize, Game& game);
	virtual ~Server();

	void								setListening(bool enable);
	void								executionThread();
	void								tick();
	sf::Time							now() const;

	void								handleIncomingPacket(RemotePeer& receivingPeer);

	void								handleIncomingConnections();
	void								handleDisconnections();

	void sendToClient(sf::Packet& packet);
};
//private:



//	void								informWorldState(sf::TcpSocket& socket);
//	void								broadcastMessage(const std::string& message);
//	void								sendToAll(sf::Packet& packet);
//	void								updateClientState();


//private:
//	sf::Time							mClientTimeoutTime;

//	std::size_t							mMaxConnectedPlayers;
//	std::size_t							mConnectedPlayers;

//	float								mWorldHeight;
//	sf::FloatRect						mBattleFieldRect;
//	float								mBattleFieldScrollSpeed;

//	std::size_t							mAircraftCount;
//	std::map<sf::Int32, AircraftInfo>	mAircraftInfo;

//	std::vector<PeerPtr>				mPeers;
//	sf::Int32							mAircraftIdentifierCounter;
//	bool								mWaitingThreadEnd;

//	sf::Time							mLastSpawnTime;
//	sf::Time							mTimeForNextSpawn;
