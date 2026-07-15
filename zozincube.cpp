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

using PixelMap = std::array<std::array<bool, (int)h>, (int)w>;

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

void	 	draw_box(double dz, double angle, PixelMap &map);
void	 	draw_screen(std::ofstream &f, PixelMap map);
void		clear_screen();
void	 	handle_line(Coord3D coord1, Coord3D coord2, double dz, double angle, PixelMap &map);
void	 	handle_point(Coord3D point, double dz, double angle, PixelMap &map);
std::ofstream	initFrame(int number);
void	 	mark_line(Coord coord1, Coord coord2, PixelMap &map);
void	 	mark_point(Coord coord, int size, PixelMap &map);
Coord	 	normalize(Coord coord);
Coord	 	project(Coord3D coord);
void	 	project_line_to_screen(Coord3D coord1, Coord3D coord2, PixelMap &map);
void	 	project_point_to_screen(Coord3D coord, PixelMap &map);
Coord3D	 	rotate_point_xz(Coord3D coord, double angle);
Coord3D	 	translate_point(Coord3D coord, double dz);

int main()
{
	double dz = 1.0;
	double angle = 0.0;
	for (int t=0; t < 240; t++)
	{
		dz += 1*dt;
		angle += M_PI*dt;

		std::ofstream f = initFrame(t);
		PixelMap map{};

               // for (Coord3D point : vs)
               //  {
               //  	handle_point(point, dz, angle, map);
               //  }

		draw_box(dz, angle, map);

		draw_screen(f, map);
	}
	return 0;
}

void draw_box(double dz, double angle, PixelMap &map)
{
	handle_line(vs[0], vs[1], dz, angle, map);
	handle_line(vs[1], vs[2], dz, angle, map);
	handle_line(vs[2], vs[3], dz, angle, map);
	handle_line(vs[3], vs[0], dz, angle, map);

	handle_line(vs[4], vs[5], dz, angle, map);
	handle_line(vs[5], vs[6], dz, angle, map);
	handle_line(vs[6], vs[7], dz, angle, map);
	handle_line(vs[7], vs[4], dz, angle, map);

	handle_line(vs[4], vs[0], dz, angle, map);
	handle_line(vs[5], vs[1], dz, angle, map);
	handle_line(vs[6], vs[2], dz, angle, map);
	handle_line(vs[7], vs[3], dz, angle, map);
}

void handle_point(Coord3D point, double dz, double angle, PixelMap &map)
{
	point = rotate_point_xz(point, angle);
	point = translate_point(point, dz);
	project_point_to_screen(point, map);
}

void handle_line(Coord3D coord1, Coord3D coord2, double dz, double angle, PixelMap &map)
{
	coord1 = rotate_point_xz(coord1, angle);
	coord1 = translate_point(coord1,dz);

	coord2 = rotate_point_xz(coord2, angle);
	coord2 = translate_point(coord2,dz);

	project_line_to_screen(coord1, coord2, map);
}

void project_point_to_screen(Coord3D coord, PixelMap &map)
{
	mark_point(normalize(project(coord)), 20, map);
}

void project_line_to_screen(Coord3D coord1, Coord3D coord2, PixelMap &map)
{
	Coord normalized_coord = normalize(project(coord1));
	Coord normalized_coord2 = normalize(project(coord2));

	normalized_coord.x = std::floor(normalized_coord.x);
	normalized_coord.y = std::floor(normalized_coord.y);
	normalized_coord2.x = std::floor(normalized_coord2.x);
	normalized_coord2.y = std::floor(normalized_coord2.y);

	mark_line(normalized_coord, normalized_coord2, map);
	mark_line(normalized_coord2, normalized_coord, map);
}

void mark_point(Coord coord, int size, PixelMap &map)
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

void mark_line(Coord coord1, Coord coord2, PixelMap &map)
{
	double slope {};
	if (static_cast<int>(coord1.x) != static_cast<int>(coord2.x))
	{
		slope = (coord2.y - coord1.y) / (coord2.x - coord1.x);
	}

	double intercept {};
	intercept = coord1.y - slope * coord1.x;

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

Coord normalize(Coord coord)
{
	return {(coord.x+1)/2.0*w, 
		(1 - (coord.y+1)/2.0)*h};
}



Coord project(Coord3D coord)
{
	return {coord.x/coord.z, coord.y/coord.z};
}

Coord3D translate_point(Coord3D coord, double dz)
{
	return {coord.x, coord.y, coord.z+dz};
}

Coord3D rotate_point_xz(Coord3D coord, double angle)
{
	const double c = std::cos(angle);
	const double s = std::sin(angle);
	return {
		coord.x*c-coord.z*s,
		coord.y,
		coord.x*s+coord.z*c
	};
}

std::ofstream initFrame(int number)
{
	std::string fileName = "output/frame-" + std::to_string(number) + ".ppm";
	std::ofstream f{fileName};

	if (!f) 
	{
		std::cerr << "Error: failed to create or open file\n";
		std::exit(1);
	}

	f << "P6\n";
	f << w << " " << h << "\n";
	f << "255\n";

	return f;
}


void draw_screen(std::ofstream &f, PixelMap map)
{
	for (int y=0; y < h; y++)
	{
		for (int x=0; x < w; x++)
		{
			if (map[static_cast<size_t>(x)][static_cast<size_t>(y)]) 
			{
				f.put(static_cast<char>(0xFF));
				f.put(static_cast<char>(0x10));
				f.put(static_cast<char>(0x10));
			}
			else
			{
				f.put(static_cast<char>(0x10));
				f.put(static_cast<char>(0x10));
				f.put(static_cast<char>(0x10));
			}
		}
	}
}

// void clear_screen()
// {
// 	for (int x=0; x < w; x++)
// 	{
// 		map[x] = (false);
// 	}
// }

