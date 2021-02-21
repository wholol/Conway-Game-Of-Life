#pragma once
#include <SFML\Graphics.hpp>
#include <string>
#include "RenderGrid.h"
#include "Timer.h"

class Game {			//game class. should have render, update/events, quit and initialize.

public:
	Game(int screenwidth, int screenheight, const std::string& title, int framerate);	//game constructor
	Game &operator=(Game &other) = delete;				//prevent assigning game object
	Game(const Game& other) = delete;					//no deep copying a game object
	Game(Game &&other) = delete;						//no moving game object

	void update();					//update logic
	void render();					//rende r
	bool quit();					//quit the window


private:
	static constexpr int cellsize = 5;
	static constexpr int numcells_x = 12800;
	static constexpr int numcells_y = 10000;
	int num_threads = std::thread::hardware_concurrency();
	static constexpr int task_granularity = 10000;
	static constexpr int generation_limit = 300;
	double OffSetX = 0.0, OffSetY = 0.0;

	GameOfLife g;
	RenderGrid visible_grid;
	std::string strategy = "basic_onethread";
	Timer time;
	bool quitgame = false;			//quit game
	bool MainMenu = true;
	
	
	
	sf::Event event;					//events class
	sf::RenderWindow window;		//windows class
	bool GameOver = false;				//gameover boolean


};