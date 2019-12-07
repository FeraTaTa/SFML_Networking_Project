#pragma once
#include "State.h"

const float ARENADEPTH = 1000;
const float CLIENTDEPTH = 0.95*ARENADEPTH;
const float SERVERDEPTH = 0.05*ARENADEPTH;
const float BALLSTARTDEPTH = SERVERDEPTH;

const int TOWARDSERVER = -1;
const int TOWARDCLIENT = 1;

const float CONVERTTORADIANS = 3.14159265359 / 180;

const float MAXBALLANGLE = 65.f;

	class GameState :
	public State
{
private:

public:
	GameState(sf::RenderWindow* window);
	virtual ~GameState();


	//functions
	void updateKeybinds(const float& dt);
	void update(const float dt);
	void render(sf::RenderTarget* target = nullptr);

};
