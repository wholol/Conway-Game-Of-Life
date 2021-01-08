#pragma once
#include <vector>
#include "ThreadPool.h"
#include <SFML/Graphics.hpp>

class GameOfLife
{

public:
	GameOfLife(int screenwidth, int screenheight,int cellsize,int numcells_x , int numcells_y,int task_granularity,int num_threads,int generation_limit);
	
	void ComputeState_basic_onethread();
	void ComputeState_SIMD_onethread();
	bool GenerationLimitReached();
	void ComputState_basic_multithread();
	void ComputState_SIMD_multithread();
	int getGenerationNum() const;
	uint8_t& GetOutputCells(int x, int y);

private:
	int task_granularity;
	ThreadPool pool;
	int screenwidth, screenheight;
	std::vector<uint8_t> output;		
	std::vector<uint8_t> state;
	std::vector<uint8_t> neighbours;
	int numcells_x, numcells_y;
	int cellsize;
	int generation_limit;
	int generations = 0;
	void ComputeState_basic_impl(int xstart, int xend, int ystart, int yend);
	void ComputeState_SIMD_impl(int xstart, int xend, int ystart, int yend);
};