#pragma once
#include <chrono>
#include <iostream>
#include <string>
class Timer
{
private:
	
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> end;
	long long endtp;
	long long starttp;
	long long dt;

	long long total_time = 0;
	std::string benchmark_location;
public:

	Timer()
	{
		start = std::chrono::high_resolution_clock::now();
		starttp = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
	}

	Timer(const std::string& benchmark_location)
		:benchmark_location(benchmark_location)
	{}

	//custom start point
	void start_timer() {
		start = std::chrono::high_resolution_clock::now();
		starttp = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
	}

	auto get_dt()
	{
		dt = endtp - starttp;
		return dt;
	}

	void print_dt()
	{
		double t = get_dt() * 1e-6;
		if (!benchmark_location.empty()) {
			std::cout << "delta time: " << t << "from location: " << benchmark_location << std::endl;
		}
		else
		{
			std::cout << "delta time: " << get_dt() * 1e-6 << std::endl;
		}
	}

	void end_timer()
	{
		end = std::chrono::high_resolution_clock::now();
		endtp = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();
	}

	void accumulate()
	{
		total_time += (endtp - starttp);
	}

	long long get_total_time()
	{
		return total_time;
	}

	void print_total_time()
	{
		if (!benchmark_location.empty()) {
			std::cout << "total time: " << total_time * 1e-6 << "from location: " << benchmark_location << std::endl;
		}
		else
		{
			std::cout << "total time: " << total_time * 1e-6 << std::endl;
		}
	}

	~Timer()
	{
		end = std::chrono::high_resolution_clock::now();
	}




};