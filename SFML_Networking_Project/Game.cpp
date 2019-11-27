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
	this->dt = this->dtClock.restart();
	//system("cls");
	//std::cout << this->dt << std::endl;
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
	//listOfCircles[0].setPosition(mousePos.x-listOfCircles[0].getRadius(), mousePos.y - listOfCircles[0].getRadius());
	myPaddle.setPosition(mousePos.x, mousePos.y);
	//std::cout << "position set to: x" << mousePos.x << " y" << mousePos.y << std::endl;
	
	//if (!this->states.empty()) {
	//	this->states.top()->update(this->dt);

	//	/*if (this->states.top()->getQuit()) {
	//		delete this->states.top();
	//		this->states.pop();
	//	}*/
	//}
}
void Game::setEnemyPaddlePosition(sf::Vector2f position) {
	
	enemyPaddle.setPosition(position.x, position.y);
}

sf::Vector2f Game::getMyPaddlePositon() {
	return myPaddle.getPosition();
}

void Game::render()
{
	this->window->clear();
	
	//render items
	if (!this->states.empty()) {
		this->states.top()->render();
	}

	this->window->draw(enemyPaddle);
	this->window->draw(listOfCircles[0]);
	this->window->draw(myPaddle);

	this->window->display();
}

void Game::run()
{
	setupGameObjects();
	selectNetworkState();
	startNetwork();

	//main loop
	while (this->window->isOpen())
	{
		this->updateDT();
		this->update();
		this->render();
		if (isHost) {
			//server loop is other thread
			
		}
		else {
			//client loop
			mGameClient->update(dt);
		}
	}
}

void Game::selectNetworkState()
{
	std::string netState;
	std::cout << "Are you the server (S/s) or the Client (C/c)? \n";
	do {
		netState.clear();
		std::cin >> netState;
		if (netState == "s" || netState == "S") {
			isHost = true;
			break;
		}
		else if (netState == "c" || netState == "C") {
			isHost = false;
			break;
		}
		std::cout << "Invalid input, try again. \n";
	} while (true);
	std::cout << netState << " " << "Server = "<< std::boolalpha << isHost << std::noboolalpha << std::endl;
}

void Game::startNetwork()
{
	sf::IpAddress ip;
	if (isHost) {
		//start server thread
		//myPaddle = serverpaddle - pass this info
		//enemypaddle = client paddle
		ip = "127.0.0.1";
		mGameServer.reset(new Server(ip, ServerPort, window->getSize(), *this));
	}
	else {
		//start client thread
		//myPaddle = client paddle - pass this info
		//enemypaddle = serverpaddle
		ip = "127.0.0.1";
		mGameClient.reset(new Client(ip, ServerPort, *this));
	}
	
}

void Game::setupGameObjects()
{
	//setup ball
	sf::CircleShape ball(100.f);
	ball.setFillColor(sf::Color::Green);
	listOfCircles.push_back(ball);


	//paddles defined by the 32x32 sprite which is scaled up 10x
	sf::IntRect cropShape(30, 90, 260, 140);
	//			  x start, y start, x length, y length
	sf::Color translucent = sf::Color(255, 255, 255, 180);

	//setup player paddle
	if (!bluPaddle.loadFromFile("Resources/bluPaddle.png")) {
		std::cout << "failed to load sprite" << std::endl;
	}
	myPaddle.setTexture(bluPaddle);
	myPaddle.setTextureRect(cropShape);
	//change the origin of the paddle to be (0,0)
	sf::Vector2f bluSpriteSize(cropShape.width, cropShape.height);
	myPaddle.setOrigin((bluSpriteSize.x / 2), (bluSpriteSize.y / 2));
	myPaddle.setColor(translucent);

	//setup enemy paddle
	if (!redPaddle.loadFromFile("Resources/redPaddle.png")) {
		std::cout << "failed to load sprite" << std::endl;
	}
	enemyPaddle.setTexture(redPaddle);
	enemyPaddle.setTextureRect(cropShape);
	//change the origin of the paddle to be (0,0)
	sf::Vector2i redSpriteSize(cropShape.width, cropShape.height);
	enemyPaddle.setOrigin(redSpriteSize.x / 2, redSpriteSize.y / 2);
	//set paddle position to the centre of the window
	sf::Vector2f windowSize = (sf::Vector2f)this->window->getSize();
	enemyPaddle.setPosition(windowSize.x / 2, windowSize.y / 2);
	enemyPaddle.setColor(translucent);
}
