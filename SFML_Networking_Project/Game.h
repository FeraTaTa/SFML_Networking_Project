#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <ctime>
#include <stdlib.h>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

class Game
{
private:
	//variables
	sf::RenderWindow *window;
	sf::Event sfEvent;
	
	sf::Clock dtClock;
	float dt;

	//initialization
	void initWindow();

public:
	//constructor/destructor
	Game();
	virtual ~Game();

	//Functions
	void updateDT();
	void updateSFMLEvents();
	void update();
	void render();
	void run();

};

#endif