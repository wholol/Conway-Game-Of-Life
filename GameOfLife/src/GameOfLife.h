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
	void ComputeState_SIMD_naive();
	void ComputState_Threaded();
	int& GetOutputCells(int x, int y);
	sf::Color& GetCellColor(int x ,int y);
	~GameOfLife();

private:
	int numthreads = std::thread::hardware_concurrency();	//intiailzie num threads before thradpool class or it wont work (threadpool class takes this in as a consturctor)
	ThreadPool pool;
	int screenwidth, screenheight;
	std::vector<int> output;		
	std::vector<int> state;
	std::vector<int> neighbours;
	std::vector<sf::Color> cellcolor;
	std::unordered_map<int, sf::Color> threadcolor;

	uint8_t* output_arr = nullptr;
	uint8_t* state_arr = nullptr;
	uint8_t* neighbour_arr = nullptr;

	std::vector<int> testthreadnum;
	int numcells_x, numcells_y;
	int cellsize;
	void ComputeState_Threaded(int xstart, int xend, int ystart, int yend, int thread_num);
	void ComputeState_SIMD_Threaded(int xstart, int xend, int ystart, int yend, int threadnum);
	void count_neighours();
};