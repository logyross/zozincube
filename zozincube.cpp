#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <cmath>
#define _USE_MATH_DEFINES

constexpr double width 	= 16*60;
constexpr double height = 16*60;
constexpr double fps 	= 60;
constexpr double dt 	= 1/fps;
constexpr int point_size= 10;
constexpr int frame_cnt = 120;
constexpr bool do_rotate= true;
constexpr bool do_move	= true;

// 2D boolean array that represents the current state of the screen (true means draw red) 
using PixelMap = std::array<std::array<bool, static_cast<int>(height)>, static_cast<int>(width)>;
static PixelMap map{};

struct Coord {
	double x{};
	double y{};
};

struct Coord3D {
	double x{};
	double y{};
	double z{};
};

// vertices that represent the box in 3D space
constexpr Coord3D vertices[] {
	{ 0.25,  0.25,   0.25},
	{-0.25,  0.25,   0.25},
	{-0.25, -0.25,   0.25},
	{ 0.25, -0.25,   0.25},

	{ 0.25,  0.25,  -0.25},
	{-0.25,  0.25,  -0.25},
	{-0.25, -0.25,  -0.25},
	{ 0.25, -0.25,  -0.25},
};

// would be better to use this to determine the faces
[[maybe_unused]] const int faces[6][4] {
	{0, 1, 2, 3},
	{4, 5, 6, 7},
	{4, 0},
	{5, 1},
	{6, 2},
	{7, 3}
};

void	 	draw_box(double dz, double angle);
void	 	draw_screen(std::ofstream &f);
void		clear_screen();

void	 	handle_line(Coord3D coord1, Coord3D coord2, double dz, double angle);
void	 	handle_point(Coord3D point, double dz, double angle);

void	 	mark_line(Coord coord1, Coord coord2);
void	 	mark_point(Coord coord, int size);

void	 	project_line_to_screen(Coord3D coord1, Coord3D coord2);
void	 	project_point_to_screen(Coord3D coord);

constexpr Coord 		project(Coord3D coord);
constexpr Coord3D	 	rotate_point_xz(Coord3D coord, double angle);
constexpr Coord3D	 	translate_point_z(Coord3D coord, double dz);

std::ofstream	initFrame(int number);


int main()
{

	double dz = 1.0;
	double angle = 0.0;
	for (int t=0; t < frame_cnt; t++) {
		if constexpr (do_move) {
			dz += 1*dt;
		}

		if constexpr (do_rotate) {
			angle += M_PI*dt;
		}

		std::ofstream f = initFrame(t);

		draw_box(dz, angle);

		draw_screen(f);
		clear_screen();
	}
	return 0;
}

// TODO: the faces are hard coded. we should read this from an array or something
void draw_box(double dz, double angle)
{
	for (Coord3D point : vertices) {
		handle_point(point, dz, angle);
	}

	handle_line(vertices[0], vertices[1], dz, angle);
	handle_line(vertices[1], vertices[2], dz, angle);
	handle_line(vertices[2], vertices[3], dz, angle);
	handle_line(vertices[3], vertices[0], dz, angle);

	handle_line(vertices[4], vertices[5], dz, angle);
	handle_line(vertices[5], vertices[6], dz, angle);
	handle_line(vertices[6], vertices[7], dz, angle);
	handle_line(vertices[7], vertices[4], dz, angle);

	handle_line(vertices[4], vertices[0], dz, angle);
	handle_line(vertices[5], vertices[1], dz, angle);
	handle_line(vertices[6], vertices[2], dz, angle);
	handle_line(vertices[7], vertices[3], dz, angle);
}

// determine the next position of the 3D points and lines then project them on the screen
void handle_point(Coord3D point, double dz, double angle)
{
	point = rotate_point_xz(point, angle);
	point = translate_point_z(point, dz);
	project_point_to_screen(point);
}
 
void handle_line(Coord3D coord1, Coord3D coord2, double dz, double angle)
{
	coord1 = rotate_point_xz(coord1, angle);
	coord1 = translate_point_z(coord1,dz);

	coord2 = rotate_point_xz(coord2, angle);
	coord2 = translate_point_z(coord2,dz);

	project_line_to_screen(coord1, coord2);
}

// project 3D coords into 2D and mark them on the map
void project_point_to_screen(Coord3D coord)
{
	Coord point = project(coord);
	// centering the point (otherwise it defaults to upper left of the box)
	point.x = point.x - point_size/2;
	point.y = point.y - point_size/2;
	mark_point(point, point_size);
}

void project_line_to_screen(Coord3D coord1, Coord3D coord2)
{
	Coord normalized_coord = project(coord1);
	Coord normalized_coord2 = project(coord2);

	// because mark_line() is pretty hacky, we need to draw the same line twice from both directions.
	mark_line(normalized_coord, normalized_coord2);
	mark_line(normalized_coord2, normalized_coord);
}

// mark a "size" sized box at coord.
void mark_point(Coord coord, int size)
{
	for (double x = coord.x; x < coord.x + size; x++) {
		for (double y = coord.y; y < coord.y + size; y++) {
			if (x < width && x >= 0
			&& y < height && y >= 0) {
				map[static_cast<size_t>(x)][static_cast<size_t>(y)] = true;
			}
		}
	}
}

// find all the points that connect coord1 and coord2 and mark them.
// Hacky, there's probably a better way to do this.
void mark_line(Coord coord1, Coord coord2)
{
	double slope {};
	if (static_cast<int>(coord1.x) != static_cast<int>(coord2.x)) {
		slope = (coord2.y - coord1.y) / (coord2.x - coord1.x);
	}
	
	double intercept {};
	intercept = coord1.y - slope * coord1.x;

	if (static_cast<int>(coord1.x) != static_cast<int>(coord2.x)) {
		for (double i = coord1.x; i <= coord2.x; i++) {
			size_t x = static_cast<size_t>(i);
			size_t y = static_cast<size_t>(slope * i + intercept);
			map[x][y] = true;
		}
	} else {
		for (double i = coord1.y; i <= coord2.y; i++) {
			size_t x = static_cast<size_t>(coord1.x);
			size_t y = static_cast<size_t>(i);
			map[x][y] = true;
		}
	
	}
}

// project the 3D point into a 2D coordinate on screen 
// 3D coords range between (-1, 1), 2D coords range between (0, height|width)
constexpr Coord project(Coord3D coord)
{
	Coord projected_coord = {coord.x/coord.z, coord.y/coord.z};
	projected_coord.x = (projected_coord.x+1)/2.0*width;
	projected_coord.y = (1 - (projected_coord.y+1)/2.0)*height; 
	return projected_coord;

}

// 3D point movement functions
constexpr Coord3D translate_point_z(Coord3D coord, double dz)
{
	return {coord.x, coord.y, coord.z+dz};
}

constexpr Coord3D rotate_point_xz(Coord3D coord, double angle)
{
	const double c = std::cos(angle);
	const double s = std::sin(angle);
	return {
		coord.x*c-coord.z*s,
		coord.y, // box will rotate on the y axis, so it stays the same
		coord.x*s+coord.z*c
	};
}

// create ppm file for current frame
std::ofstream initFrame(int number)
{
	std::string fileName = "output/frame-" + std::to_string(number) + ".ppm";
	std::ofstream f{fileName};

	if (!f) {
		std::cerr << "Error: failed to create or open file\n";
		std::exit(1);
	}

	f << "P6\n";
	f << width << " " << height << "\n";
	f << "255\n";

	return f;
}

// write pixel color codes on the ppm file based on the current state of PixelMap
void draw_screen(std::ofstream &f)
{
	for (int y=0; y < height; y++) {
		for (int x=0; x < width; x++) {
			size_t x_coord = static_cast<size_t>(x);
			size_t y_coord = static_cast<size_t>(y);

			if (map[x_coord][y_coord]) {
				f.put(0xFF);
				f.put(0x10);
				f.put(0x10);
			} else {
				f.put(0x10);
				f.put(0x10);
				f.put(0x10);
			}
		}
	}
}

// clear the map for the next frame
void clear_screen()
{
	for (int y=0; y < height; y++) {
		for (int x=0; x < width; x++) {
			size_t x_coord = static_cast<size_t>(x);
			size_t y_coord = static_cast<size_t>(y);
			map[x_coord][y_coord] = false;
		}
	}
}

