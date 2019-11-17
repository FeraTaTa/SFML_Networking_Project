#pragma once
#include <vector>

#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window/Mouse.hpp>


class State
{
private:
	sf::RenderWindow* window;
	std::vector<sf::Texture> textures;
	bool quit;
	const bool& getQuit() const;

public:
	State(sf::RenderWindow* window);
	virtual ~State();
	//set to pure virtual - can't create staet object but inherited class will need these defined
	virtual void updateKeybinds(const float& dt) = 0;
	virtual void checkForQuit();
	virtual void update(const float dt) = 0;
	virtual void render(sf::RenderTarget* target = nullptr) = 0;
};

