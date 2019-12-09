#pragma once

const unsigned short ServerPort = 5000;

// Structure to store information about current paddle state
struct PaddleInfo
{
	sf::Vector2f				position;
};

namespace packetClient
{
	// Packets originated in the client
	enum PacketType
	{
		StartGame,
		PositionUpdate,
		BallReverse,
		BallMissed,
		Quit
	};
}

namespace packetServer
{
	// Packets originated in the server
	enum PacketType
	{
		StartGame,
		UpdateClientState,
		BallReverse,
		BallMissed,
		Quit
	};
}
