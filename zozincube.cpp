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

// decide if coord1 or coord2
void markLine(Coord coord1, Coord coord2, std::array<std::array<bool, (int)h>, (int)w> &map)
{
	//std::cout << "1X = " << coord1.x << " 1Y = " << coord1.y << " 2X = " << coord2.x << " 2Y = " << coord2.y << "\n";
	double slope {};
	if (static_cast<int>(coord1.x) != static_cast<int>(coord2.x))
	{
		slope = (coord2.y - coord1.y) / (coord2.x - coord1.x);
	}

	double intercept {};
	if (slope != 0.0)
	{
		intercept = coord1.y - slope * coord1.x;
	}

	//std::cout << "slope = " << slope << " intercep = " << intercept << "\n";
	if (static_cast<int>(coord1.x) != static_cast<int>(coord2.x))
	{
		for (double i = coord1.x; i <= coord2.x; i++)
		{
			double j = slope * i + intercept;
			//std::cout << "i = " << i << " j = " << j << "\n";
			map[static_cast<size_t>(i)][static_cast<size_t>(j)] = true;
		}
	}
	else
	{

		for (double i = coord1.y; i <= coord2.y; i++)
		{
			double j = coord1.x;
			//std::cout << "i = " << i << " j = " << j << "\n";
			map[static_cast<size_t>(j)][static_cast<size_t>(i)] = true;
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
	//std::cout << "-- HANDLE POINT -- \n";
	Coord3D rotated_point = rotate_xz(coord, angle);
	Coord3D translated_point = translatePoint(rotated_point, dz);
	//std::cout << "TRANSLATED -> X: " << translated_point.x << " Y: " << translated_point.y << "\n";
	Coord projected_coord = project(translated_point);
	//std::cout << "PROJECTED -> X: " << projected_coord.x << " Y: " << projected_coord.y << "\n";
	Coord normalized_coord = normalize(projected_coord);
	//std::cout << "NORMALIZED -> X: " << normalized_coord.x << " Y: " << normalized_coord.y << "\n";
	markPoint(normalized_coord, 20, map);
}

void handleLine(Coord3D coord1, Coord3D coord2, double dz, double angle, std::array<std::array<bool, (int)h>, (int)w> &map)
{
	Coord3D rotated_point = rotate_xz(coord1, angle);
	Coord3D translated_point = translatePoint(rotated_point, dz);
	Coord projected_coord = project(translated_point);
	Coord normalized_coord = normalize(projected_coord);

	Coord3D rotated_point2 = rotate_xz(coord2, angle);
	Coord3D translated_point2 = translatePoint(rotated_point2, dz);
	Coord projected_coord2 = project(translated_point2);
	Coord normalized_coord2 = normalize(projected_coord2);

	normalized_coord.x = std::floor(normalized_coord.x);
	normalized_coord.y = std::floor(normalized_coord.y);
	normalized_coord2.x = std::floor(normalized_coord2.x);
	normalized_coord2.y = std::floor(normalized_coord2.y);
	markLine(normalized_coord, normalized_coord2, map);
	markLine(normalized_coord2, normalized_coord, map);
}


const Coord3D vs[]
{
	{ 0.25,  0.25,   0.25},
	{-0.25,  0.25,   0.25},
	{-0.25, -0.25,   0.25},
	{ 0.25, -0.25,   0.25},

	{ 0.25,  0.25,  -0.25},
	{-0.25,  0.25,  -0.25},
	{-0.25, -0.25,  -0.25},
	{ 0.25, -0.25,  -0.25},
};

[[maybe_unused]] const int fs[2][4] =
{
	{0, 1, 2, 3},
	{4, 5, 6, 7}
};

int main()
{
	double dz = 1.0;
	double angle = 0.0;
	std::cout << "W: " << w << " H: " << h << "\n";
	for (int t=0; t < 240; t++)
	{
		//dz += 1*dt;
		angle += M_PI*dt;
    		std::string fileName = "output/test-" + std::to_string(t) + ".ppm";
		std::ofstream f{fileName};

		if (!f) 
		{
			std::cerr << "failed to open file.\n";
			return 1;
		}

		f << "P6\n";

		std::array<std::array<bool, (int)h>, (int)w> map{};

		// for (Coord3D point : vs)
		// {
		// 	handlePoint(point, map, dz, angle);
		// }

		handleLine(vs[0], vs[1], dz, angle, map);
		handleLine(vs[1], vs[2], dz, angle, map);
		handleLine(vs[2], vs[3], dz, angle, map);
		handleLine(vs[3], vs[0], dz, angle, map);

		handleLine(vs[4], vs[5], dz, angle, map);
		handleLine(vs[5], vs[6], dz, angle, map);
		handleLine(vs[6], vs[7], dz, angle, map);
		handleLine(vs[7], vs[4], dz, angle, map);

		handleLine(vs[4], vs[0], dz, angle, map);
		handleLine(vs[5], vs[1], dz, angle, map);
		handleLine(vs[6], vs[2], dz, angle, map);
		handleLine(vs[7], vs[3], dz, angle, map);


		// for (auto arr : fs)
		// {
		// 	int s = sizeof(*arr) / sizeof(arr[0]);
		// 	for (int i=0; i < s; i++)
		// 	{
		// 		Coord3D a = vs[arr[i]];
		// 		Coord3D b = vs[arr[(i+1)%s]];
		// 		handleLine(a, b, dz, angle, map);
		// 	}
		// }

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