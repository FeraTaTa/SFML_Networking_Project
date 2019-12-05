#pragma once

const unsigned short ServerPort = 5000;



// Structure to store information about current paddle state
struct PaddleInfo
{
	sf::Vector2f				position;
	//sf::Int32					hitpoints;
	//sf::Int32                   missileAmmo;
	//std::map<sf::Int32, bool>	realtimeActions;
};




namespace packetClient
{
	// Packets originated in the client
	enum PacketType
	{
		PlayerEvent,
		PlayerRealtimeChange,
		BallReverse,
		StartGame,
		RequestCoopPartner,
		PositionUpdate,
		GameEvent,
		Quit
	};
}

namespace packetServer
{
	// Packets originated in the server
	enum PacketType
	{
		BroadcastMessage,	// format: [Int32:packetType] [string:message]
		SpawnSelf,			// format: [Int32:packetType]
		InitialState,
		PlayerEvent,
		PlayerRealtimeChange,
		PlayerConnect,
		PlayerDisconnect,
		StartGame,
		BallReverse,
		AcceptCoopPartner,
		SpawnEnemy,
		SpawnPickup,
		UpdateClientState,
		MissionSuccess
	};
}

