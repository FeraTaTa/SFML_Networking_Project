#ifndef GAME_H
#define GAME_H

#include "GameState.h"
#include "Client.h"
#include "Server.h"

class Game
{
private:
	//variables
	sf::RenderWindow *window;
	sf::Event sfEvent;
	
	sf::Clock dtClock;
	sf::Time dt;
	sf::Vector2f mousePos;
	std::vector<sf::CircleShape> listOfCircles;
	
	sf::Texture bluPaddle;
	sf::Texture redPaddle;

	sf::Sprite myPaddle;
	sf::Sprite enemyPaddle;
	bool isHost = false;


	std::stack<State*> states;
	//initialization
	void initWindow();
	void initStates();

public:
	//constructor/destructor
	Game();
	virtual ~Game();

	
	std::unique_ptr<Server> mGameServer;
	std::unique_ptr<Client> mGameClient;
	//Functions
	void updateDT();
	void updateSFMLEvents();
	void update();
	void render();
	void run();
	void setEnemyPaddlePosition(sf::Vector2f position);
	sf::Vector2f getMyPaddlePositon();
	void selectNetworkState();
	void startNetwork();
	void setupGameObjects();

};

#endif