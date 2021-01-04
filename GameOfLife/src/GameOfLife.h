#pragma once
#include <vector>
#include "ThreadPool.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "Timer.h"

class GameOfLife
{

public:
	GameOfLife(int screenwidth, int screenheight,int cellsize);
	
	void ComputeState_basic_onethread();
	void ComputeState_SIMD_onethread();

	void ComputState_basic_multithread();
	void ComputState_SIMD_multithread();
	Timer getTimer() const
	{
		return a;
	}

	uint8_t& GetOutputCells(int x, int y);

private:
	int numthreads = 10000;	//intiailzie num threads before thradpool class or it wont work (threadpool class takes this in as a consturctor)
	ThreadPool pool;
	int screenwidth, screenheight;
	std::vector<uint8_t> output;		
	std::vector<uint8_t> state;
	std::vector<uint8_t> neighbours;
	std::vector<sf::Color> cellcolor;
	std::unordered_map<int, sf::Color> threadcolor;
	Timer a;
	int numcells_x, numcells_y;
	int cellsize;
	void ComputeState_basic_impl(int xstart, int xend, int ystart, int yend, int thread_num);
	void ComputeState_SIMD_impl(int xstart, int xend, int ystart, int yend, int thread_num);
};