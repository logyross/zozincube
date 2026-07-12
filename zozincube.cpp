#include <fstream>
#include <iostream>
#include <array>
#include <string>

constexpr double w = 16*60;
constexpr double h = 9*60;
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
			if (x <= w && x >= 0
			&& y <= h && y >= 0)
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

void handle_point(Coord3D coord, std::array<std::array<bool, (int)h>, (int)w> &map)
{
	Coord projected_coord = project({coord.x, coord.y, coord.z});
	Coord normalized_coord = normalize(projected_coord);
	// std::cout << "X: " << normalized_coord.x << " Y: " << normalized_coord.y << "\n";
	// std::cout << "Z: " << 1.0 + dz << "\n";
	markPoint(normalized_coord, 20, map);
}

int main()
{
	double dz = 0.0;
	std::cout << "W: " << w << " H: " << h << "\n";
	for (int t=0; t < 100; t++)
	{
		dz += 1*dt;
    		std::string fileName = "output/test-" + std::to_string(t) + ".ppm";
		std::ofstream f{fileName};

		if (!f) 
		{
			std::cerr << "failed to open file.\n";
			return 1;
		}

		f << "P6\n";

		std::array<std::array<bool, (int)h>, (int)w> map{};

		handle_point({0.5, 0.5, 1.0 + dz}, map);
		handle_point({-0.5, 0.5, 1.0 + dz}, map);
		handle_point({0.5, -0.5, 1.0 + dz}, map);
		handle_point({-0.5, -0.5, 1.0 + dz}, map);

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