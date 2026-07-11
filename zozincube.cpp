#include <fstream>
#include <iostream>
#include <array>

constexpr int w = 16*60;
constexpr int h = 9*60;

struct Cell
{

	int color_r{};
	int color_g{};
	int color_b{};
};

struct Color 
{
	int r{};
	int g{};
	int b{};
};

struct Coord
{
	int x{};
	int y{};
};

// void fillRect(std::ofstream& f, Coord coord, Color color, int w, int h)
// {

// 	for (int i=0; i <= w; i++)
// 	{
// 		for (int j=0; j <= h; j++)
// 		{
// 			f.put(0x10);
// 			f.put(0x10);
// 			f.put(0x10);
// 		}
// 	}
// }

void markPoint(int x, int y, int size, std::array<std::array<bool, h>, w> &map)
{
	for (int i = x; i < x + size; i++)
	{
		for (int j = y; j < y + size; j++)
		{
			map[static_cast<size_t>(i)][static_cast<size_t>(j)] = true;
		}
	}
}


int main()
{
	std::ofstream f{"test.ppm"};

	if (!f) 
	{
		std::cerr << "failed to open file.\n";
		return 1;
	}

	f << "P6\n";


	std::array<std::array<bool, h>, w> map{};

	markPoint(16*30, 9*30, 10, map);
	markPoint(16*40, 9*40, 20, map);
	

	f << w << " " << h << "\n";
	f << "255\n";

	for (int y=0; y < h; y++)
	{
		for (int x=0; x < w; x++)
		{
			if (map[static_cast<size_t>(x)][static_cast<size_t>(y)]) 
			{
				f.put(0xFF);
				f.put(0x10);
				f.put(0x10);
			}
			else
			{
				f.put(0x10);
				f.put(0x10);
				f.put(0x10);
			}
		}
	}
	return 0;
}