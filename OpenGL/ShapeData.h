#pragma once

#include <vector>

using namespace std;

class ShapeData
{
public:
	vector<float> vertices;
	vector<float> colours;
	vector<unsigned short> indices;
	vector<float> offsets;
};