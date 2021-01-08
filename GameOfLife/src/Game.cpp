#include "Game.h"
#include <thread>
#include <iostream>

Game::Game(int screenwidth, int screenheight, const std::string& title, int framerate)
	:window(sf::VideoMode(screenwidth, screenheight), title)
	,g(screenwidth,screenheight,cellsize,numcells_x,numcells_y,task_granularity,num_threads,generation_limit)
	,visible_grid(screenwidth,screenheight,g,cellsize, numcells_x, numcells_y)
{}

void Game::render() {		//rendering
	visible_grid.Render(window,OffSetX,OffSetY);
}

void Game::update() {		//update game 
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		OffSetX -= 5;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		OffSetX += 5;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		OffSetY -= 5;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		OffSetY += 5;
	}

	if (g.GenerationLimitReached())
	{
		std::cout << "generation reached!.Logging data to csv." << std::endl;
		time.log_data();
		quitgame = true;
	}

	else {
		time.start_timer();
		g.ComputState_SIMD_multithread();
		time.end_timer();
		std::cout << g.getGenerationNum() << std::endl;
		time.print_dt();
		window.clear();
	}
	
}


bool Game::quit()			//call quit game

//DO NOT DELETE CONTENTS OF THIS FUNCTION TO PREVENT AN UNRESPONSIV SCREEN//
{
	while (window.pollEvent(event)) {		//NON BLOCKING FUNCTION, WILL NOT GET STUCK IN WHILE LOOP.
		if (event.type == sf::Event::Closed) {
			window.close();
			quitgame = true;
		}
	}
	return quitgame;
	
}
