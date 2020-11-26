#include "GameOfLife.h"
#include <thread>
#include <future>
#include <iostream>
#include "Timer.h"

GameOfLife::GameOfLife(int screenwidth, int screenheight,int cellsize)
	:pool(numthreads),screenheight(screenheight), screenwidth(screenwidth), 
	 cellsize(cellsize)
{
	numcells_x = screenwidth / cellsize;
	numcells_y =screenheight / cellsize;
	int vector_size = (numcells_x * numcells_y);
	state.reserve(vector_size);
	output.reserve(vector_size);
	
	for (int i = 0; i < vector_size; ++ i)
	{
		state.emplace_back(0);
		output.emplace_back(rand() % 2);		//init state to zero.
		cellcolor.emplace_back(sf::Color::Black);
	}

	//ensure border cells are dead
	for (int y = 0; y < numcells_y  ; ++y)
	{
		output[numcells_x * y] = 0;
		output[(numcells_x - 1) + numcells_x * y] = 0;
		//cellcolor[numcells_x * y] = sf::Color::Cyan;
		//cellcolor[(numcells_x - 1) + numcells_x * y] = sf::Color::Cyan;
	}

	//ensure border cells are dead
	for (int x = 0; x < numcells_x ; ++x)
	{
		output[x] = 0;
		output[x + numcells_x * (numcells_y - 1)] = 0;
		//cellcolor[x] = sf::Color::Cyan;
		//cellcolor[x + numcells_x * (numcells_y - 1)] = sf::Color::Cyan;
	}
	

	threadcolor.insert(std::make_pair(1, sf::Color::Red));
	threadcolor.insert(std::make_pair(2, sf::Color::Cyan));
	threadcolor.insert(std::make_pair(3, sf::Color::Blue));
	threadcolor.insert(std::make_pair(4, sf::Color::Yellow));
	threadcolor.insert(std::make_pair(5, sf::Color::Magenta));
	threadcolor.insert(std::make_pair(6, sf::Color::Green));
	threadcolor.insert(std::make_pair(7, sf::Color(255, 165, 0)));
	threadcolor.insert(std::make_pair(8, sf::Color(128, 0, 128)));
}

void GameOfLife::ComputeState()
{
	auto cell = [=](int x, int y) { return  output[x + numcells_x * y]; };		
	int neighbours;
	static constexpr int thread_num = 1;
	for (int y = 1; y < numcells_y - 1; ++y)
	{
		for (int x = 1; x < numcells_x - 1; ++x)
		{
			neighbours = cell(x + 1, y + 1) + cell(x - 1, y - 1) + cell(x + 1, y) + cell(x - 1, y) + cell(x + 1, y - 1) + cell(x - 1, y + 1) + cell(x, y + 1) + cell(x, y - 1);

			if (cell(x, y)) {
				state[x + numcells_x * y] = neighbours == 2 || neighbours == 3;
			}
			else {
				state[x + numcells_x * y] = neighbours == 3;
			}

			auto find = threadcolor.find(thread_num);
			
			if (find != threadcolor.end())
			{
				cellcolor[x + numcells_x * y] = find->second;
			}
			else {
				throw std::runtime_error("cell was not computed");
			}

		}
	}
	output.swap(state);	//update output vector
}


void GameOfLife::ComputState_Threaded()
{
	int thread_height = numcells_y / numthreads;	//ensure its even...	//(std::thread::hardware_concurrency to determine number of threads.)

	if (numcells_x % numthreads != 0)
	{
		throw std::runtime_error("number of cells divided by thread number not even!");
	}

	pool.workerdone = 0;

	for (int i = 0; i < numthreads; ++i)
	{
		static constexpr int xstart = 1;
		const int xend = numcells_x - 1;
		int ystart = (i * thread_height);
		int yend = (i + 1) * thread_height;
		int thread_num = i + 1;

		if (ystart == 0)
		{
			ystart = 1;
		}

		std::packaged_task<void()> T(std::bind(&GameOfLife::ComputeState_T, this, xstart, xend, ystart, yend, thread_num));
		pool.enqueue(std::move(T));
	}

	while (pool.workerdone < numthreads) {}	//wait for all workers to complete.
		
	output.swap(state);	//only swap after threads complete
}

std::vector<int> GameOfLife::GetOutputCells() const
{
	return output;
}

std::vector<sf::Color> GameOfLife::GetCellColor() const
{
	return cellcolor;
}

void GameOfLife::ComputeState_T(int xstart, int xend, int ystart, int yend, int thread_num)
{
	
	auto cell = [=](int x, int y) { return  output[x + numcells_x * y]; };		
	int neighbours;

	for (int y = ystart; y < yend ; ++y)
	{
		for (int x = xstart ; x < xend ; ++x)
		{
			neighbours = cell(x + 1, y + 1) + cell(x - 1, y - 1) + cell(x + 1, y) + cell(x - 1, y) + cell(x + 1, y - 1) + cell(x - 1, y + 1) + cell(x, y + 1) + cell(x, y - 1);
			
			if (output[x + numcells_x * y])	
			{
				state[x + numcells_x * y] = neighbours == 2 || neighbours == 3;
			}
			else {
				state[x + numcells_x * y] = neighbours == 3;
			}
			
			auto find = threadcolor.find(thread_num);

			if (find != threadcolor.end())
			{
				cellcolor[x + numcells_x * y] = find->second;
			}
			else {
				throw std::runtime_error("cell was not computed");
			}
			
		}
	}
}

