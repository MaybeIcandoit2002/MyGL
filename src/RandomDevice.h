#pragma once
#include <random>
class RandomDevice
{
private:
	std::random_device rd;
	std::mt19937 gen;
public:
	RandomDevice() : gen(rd()) {}
	int Int(int min, int max) {
		std::uniform_int_distribution<> distrib(min, max);
		return distrib(gen);
	}
	float Float(float min, float max) {
		std::uniform_real_distribution<float> distrib(min, max);
		return distrib(gen);
	}
};

