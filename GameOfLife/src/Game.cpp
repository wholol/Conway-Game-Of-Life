#include "Game.h"
#include <thread>
#include <iostream>

Game::Game(int screenwidth, int screenheight, const std::string& title, int framerate)
	:window(sf::VideoMode(screenwidth, screenheight), title)
	,g(screenwidth,screenheight,cellsize,numcells_x,numcells_y,task_granularity,num_threads,generation_limit)
	,renderer(screenwidth,screenheight,g,cellsize, numcells_x, numcells_y)
{}

void Game::render() {		//rendering
	
	renderer.RenderGrid(window,OffSetX,OffSetY);
	renderer.RenderUI(strategy, time.get_dt(), window);
	window.display();	
	
}

void Game::update() {		//update game 
	
	//move screen aound world
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

	//SIMD with multi thread
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		strategy ="SIMD_onethread";
		g.resetGame();
	}

	//one thread SIMD
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		strategy = "SIMD_multithread";
		time.start_timer();
		g.resetGame();
		time.end_timer();
		time.print_dt();
	}

	//default method (no SIMD, no thread)
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		strategy = "basic_onethread";
		g.resetGame();
	}

	//mulyithread, no SIMD
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		strategy = "basic_multithread";
		g.resetGame();
	}

	//press L to log the data into a csv file.
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
	{
		logData = true;
	}

	if (g.GenerationLimitReached() && logData)
	{
		std::cout << "generation reached!.Logging data to csv." << std::endl;
		time.log_data(strategy);
		g.resetGame();
	}

	else {
		time.start_timer();

		if (strategy == "SIMD_onethread")
		{
			g.ComputeState_SIMD_onethread();
		}

		else if (strategy == "SIMD_multithread")
		{
			g.ComputState_SIMD_multithread();
		}

		else if (strategy == "basic_multithread")
		{
			g.ComputState_basic_multithread();
		}
		
		else if (strategy == "basic_onethread")
		{
			g.ComputeState_basic_onethread();
		}

		time.end_timer();

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
