#pragma once

#include <vector>

using namespace std;

//Provides a medium to transport manual geometry
class ShapeData
{
public:
	vector<float> vertices;
	vector<float> colours;
	vector<unsigned short> indices;
};