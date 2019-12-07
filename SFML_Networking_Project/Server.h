#pragma once
#include "GameState.h"

class Game;
class Ball;

class Server
{
private:
	sf::TcpSocket mSocket;
	sf::TcpListener mListenerSocket;
	sf::Thread mThread;
	sf::Clock mClock;
	bool mListeningState;
	sf::FloatRect mBattleFieldRect;
	std::size_t mMaxConnectedPlayers;
	std::size_t mConnectedPlayers;
	bool mConnected;
	bool mWaitingThreadEnd;
	sf::Time mClientTimeoutTime;

	Game* world;
	Ball* ballObject;

	// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
	struct RemotePeer
	{
		RemotePeer();
		sf::TcpSocket socket;
		sf::Time lastPacketTime;
		bool ready;
		bool timedOut;
	};
	typedef std::unique_ptr<RemotePeer>	PeerPtr;
	PeerPtr	mPeer;

	PaddleInfo clientPaddle;
	PaddleInfo serverPaddle;

public:
	Server(sf::IpAddress ip, unsigned short ServerPort, sf::Vector2u battlefieldSize, Game& game);
	virtual ~Server();

	void setListening(bool enable);
	void executionThread();
	void tick();
	sf::Time now() const;

	void handleIncomingPacket(RemotePeer& receivingPeer);

	void handleIncomingConnections();
	void handleDisconnections();

	void sendToClient(sf::Packet& packet);
};
