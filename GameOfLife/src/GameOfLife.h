#pragma once
#include <vector>
#include "ThreadPool.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>

class GameOfLife
{

public:
	GameOfLife(int screenwidth, int screenheight,int cellsize);
	void ComputeState();
	void ComputState_Threaded();
	std::vector<int> GetOutputCells() const;
	std::vector<sf::Color> GetCellColor() const;

private:
	int numthreads = std::thread::hardware_concurrency();	//intiailzie num threads before thradpool class or it wont work (threadpool class takes this in as a consturctor)
	ThreadPool pool;
	int screenwidth, screenheight;
	std::vector<int> output;		
	std::vector<int> state;
	std::vector<sf::Color> cellcolor;
	std::unordered_map<int, sf::Color> threadcolor;
	std::vector <int> testthreadnum;
	int numcells_x, numcells_y;
	int cellsize;
	void ComputeState_T(int xstart, int xend, int ystart, int yend, int thread_num);
};