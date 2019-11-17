#include "Game.h"

//static functions

//initialize functions
void Game::initWindow()
{
	//creates an SFML window using options from window.ini file.
	std::ifstream ifs("Config/window.ini");
	sf::VideoMode window_bounds(800,600);
	std::string title = "None";
	unsigned framerate_limit = 120;
	bool vertical_sync_enabled = false;

	if (ifs.is_open()) {
		std::getline(ifs, title);
		ifs >> window_bounds.width >> window_bounds.height;
		ifs >> framerate_limit;
		ifs >> vertical_sync_enabled;
	}
	ifs.close();
	
	this->window = new sf::RenderWindow(window_bounds, title);
	this->window->setFramerateLimit(framerate_limit);
	this->window->setVerticalSyncEnabled(vertical_sync_enabled);
}

void Game::initStates()
{
	this->states.push(new GameState(this->window));
}

//constructors/destructors


Game::Game()
{
	this->initWindow();
	this->initStates();


	
}


Game::~Game()
{
	delete this->window;

	while (!this->states.empty()) {
		delete this->states.top();
		this->states.pop();
	}
}

//Functions
void Game::updateDT()
{
	//updates the dt variable with the time it takes to update and render one frame
	this->dt = this->dtClock.restart().asSeconds();
	//system("cls");
	std::cout << this->dt << std::endl;
}

//Captures event for closing window
void Game::updateSFMLEvents()
{
	while (this->window->pollEvent(sfEvent))
	{
		if (this->sfEvent.type == sf::Event::Closed)
			this->window->close();
	}
}

void Game::update()
{
	this->updateSFMLEvents();
	mousePos = (sf::Vector2f)sf::Mouse::getPosition(*(this->window));
	listOfShapes[0].setPosition(mousePos.x-listOfShapes[0].getRadius(), mousePos.y - listOfShapes[0].getRadius());
	std::cout << "position set to: x" << mousePos.x << " y" << mousePos.y;
	//if (!this->states.empty()) {
	//	this->states.top()->update(this->dt);

	//	/*if (this->states.top()->getQuit()) {
	//		delete this->states.top();
	//		this->states.pop();
	//	}*/
	//}
}

void Game::render()
{
	this->window->clear();
	
	//render items
	if (!this->states.empty()) {
		this->states.top()->render();
	}

	this->window->draw(listOfShapes[0]);

	this->window->display();
}

void Game::run()
{
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	listOfShapes.push_back(shape);

	while (this->window->isOpen())
	{
		this->updateDT();
		this->update();
		this->render();
	}
}
