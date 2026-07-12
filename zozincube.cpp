#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <cmath>
#define _USE_MATH_DEFINES

constexpr double w = 16*60;
constexpr double h = 16*60;
constexpr double fps = 60;
constexpr double dt = 1/fps;

struct Coord 
{
	double x{};
	double y{};
};

struct Coord3D
{
	double x{};
	double y{};
	double z{};
};

Coord normalize(Coord coord)
{
	return {(coord.x+1)/2.0*w, 
		(1 - (coord.y+1)/2.0)*h};
}

void markPoint(Coord coord, int size, std::array<std::array<bool, (int)h>, (int)w> &map)
{
	for (double x = coord.x; x < coord.x + size; x++)
	{
		for (double y = coord.y; y < coord.y + size; y++)
		{
			if (x < w && x >= 0
			&& y < h && y >= 0)
			{
				map[static_cast<size_t>(x)][static_cast<size_t>(y)] = true;
			}
		}
	}
}

Coord project(Coord3D coord)
{
	return {coord.x/coord.z, coord.y/coord.z};
}

Coord3D translatePoint(Coord3D coord, double dz)
{
	return {coord.x, coord.y, coord.z+dz};
}

Coord3D rotate_xz(Coord3D coord, double angle)
{
	const double c = std::cos(angle);
	const double s = std::sin(angle);
	return {
		coord.x*c-coord.z*s,
		coord.y,
		coord.x*s+coord.z*c
	};
}

void handlePoint(Coord3D coord, std::array<std::array<bool, (int)h>, (int)w> &map, double dz, double angle)
{
	std::cout << "-- HANDLE POINT -- \n";
	Coord3D rotated_point = rotate_xz(coord, angle);
	Coord3D translated_point = translatePoint(rotated_point, dz);
	std::cout << "TRANSLATED -> X: " << translated_point.x << " Y: " << translated_point.y << "\n";
	Coord projected_coord = project(translated_point);
	std::cout << "PROJECTED -> X: " << projected_coord.x << " Y: " << projected_coord.y << "\n";
	Coord normalized_coord = normalize(projected_coord);
	std::cout << "NORMALIZED -> X: " << normalized_coord.x << " Y: " << normalized_coord.y << "\n";
	markPoint(normalized_coord, 20, map);
}



const Coord3D vs[]
{
	{ 0.25,  0.25,   0.25},
	{-0.25,  0.25,   0.25},
	{ 0.25, -0.25,   0.25},
	{-0.25, -0.25,   0.25},
	{ 0.25,  0.25,  -0.25},
	{-0.25,  0.25,  -0.25},
	{ 0.25, -0.25,  -0.25},
	{-0.25, -0.25,  -0.25},
};

int main()
{
	double dz = 1.0;
	double angle = 0.0;
	std::cout << "W: " << w << " H: " << h << "\n";
	for (int t=0; t < 60; t++)
	{
		//dz += 1*dt;
		angle += 2*M_PI*dt;
    		std::string fileName = "output/test-" + std::to_string(t) + ".ppm";
		std::ofstream f{fileName};

		if (!f) 
		{
			std::cerr << "failed to open file.\n";
			return 1;
		}

		f << "P6\n";

		std::array<std::array<bool, (int)h>, (int)w> map{};

		for (Coord3D point : vs)
		{
			handlePoint(point, map, dz, angle);
		}
		// handlePoint({0.5, 0.5, 1.0 + dz}, map, dz);
		// handlePoint({-0.5, 0.5, 1.0 + dz}, map, dz);
		// handlePoint({0.5, -0.5, 1.0 + dz}, map, dz);
		// handlePoint({-0.5, -0.5, 1.0 + dz}, map, dz);

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
	}
	return 0;
}