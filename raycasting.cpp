#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cassert>

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a=255) {
	return (a << 24) + (b << 16) + (g << 8) + r;
}

void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) {
	r = (color >> 0) & 255;
	g = (color >> 8) & 255;
	b = (color >> 16) & 255;
	a = (color >> 24) & 255;
}

void drop_ppm_image(const std::string filename, const std::vector<uint32_t>& image, const size_t w, const size_t h) {
	assert(image.size() == w * h);
	std::ofstream ofs(filename, std::ios::binary);
	ofs << "P6\n" << w << " " << h << "\n255\n";
	for (size_t i = 0; i < w * h; ++i) {
		uint8_t r, g, b, a;
		unpack_color(image[i], r, g, b, a);
		ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
	}
	ofs.close();
}

void draw_rect(std::vector<uint32_t>& img, const size_t img_w, const size_t img_h, const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color) {
	for (size_t i = 0; i < w; i++) {
		for (size_t j = 0; j < h; j++) {
			size_t cx = x + i;
			size_t cy = y + j;
			//assert(cx < img_w && cy < img_h);
			if (cx >= img_w || cy >= img_h) continue;
			img[cx + cy * img_w] = color;
		}
	}
}

int main() {
	const size_t win_w = 1024;
	const size_t win_h = 512;
	std::vector<uint32_t> framebuffer(win_w * win_h, 255);

	const size_t map_w = 16;
	const size_t map_h = 16;
	const char map[] =  "1111111111111111"\
						"1000010000000001"\
						"1000010010000001"\
						"1000010011100001"\
						"1000010000100001"\
						"1000011111100001"\
						"1000000000000001"\
						"1000000000000001"\
						"1000000000000001"\
						"1000000000000001"\
						"1000000000000001"\
						"1000001111110001"\
						"1000001000010001"\
						"1000000000000001"\
						"1000000000000001"\
						"1111111111111111";
	assert(sizeof(map) == map_w * map_h + 1);

	float player_x = 2.5;
	float player_y = 2.5;
	float player_dr = 0.9; //dir of the player
	const float fov = M_PI / 3.;

	for (size_t j = 0; j < win_h; j++) { //creating gradient rect
		for (size_t i = 0; i < win_w; i++) {
			uint8_t r = 255 * j / float(win_h);
			uint8_t g = 0;
			uint8_t b = 255 * i / float(win_w);
			framebuffer[i + j * win_w] = pack_color(r, g, b);
		}
	}

	//const size_t rect_w = win_w / map_w;
	const size_t rect_w = win_w / (map_w * 2);
	const size_t rect_h = win_h / map_h;


	for (size_t frame = 0; frame < 360; frame++) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
		player_dr += 2 * M_PI / 360;

		framebuffer = std::vector<uint32_t>(win_w * win_h, pack_color(255, 255, 255));
		for (size_t j = 0; j < map_h; j++) { //drawing over the gradient (walls)
			for (size_t i = 0; i < map_w; i++) {
				if (map[i + j * map_w] == '0') continue;
				size_t rect_x = i * rect_w;
				size_t rect_y = j * rect_h;
				draw_rect(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, pack_color(0, 255, 0));
			}
		}
		for (size_t i = 0; i < win_w / 2; i++) { //drawing cone (view of the player) / 512 rays
			float angle = player_dr - fov / 2 + fov * i / float(win_w / 2);
			for (float c = 0; c < 22.6; c += .05) { // 22.6 bcs that's the diagonal (max distce)
				float cx = player_x + c * cos(angle); //drawing one line until it hits a wall
				float cy = player_y + c * sin(angle);
				//if (map[int(cx) + int(cy) * map_w] != '0') break;

				size_t pix_x = cx * rect_w;
				size_t pix_y = cy * rect_h;
				framebuffer[pix_x + pix_y * win_w] = pack_color(128, 128, 128);
				if (map[int(cx) + int(cy) * map_w] != '0') { //drawing walls in 3D
					size_t column_h = win_h / (c * cos(angle - player_dr));
					draw_rect(framebuffer, win_w, win_h, win_w / 2 + i, win_h / 2 - column_h / 2, 1, column_h, pack_color(0, 255, 0));
					break;
				}
			}
		}

		drop_ppm_image(ss.str(), framebuffer, win_w, win_h);
	}

	//draw_rect(framebuffer, win_w, win_h, player_x * rect_w, player_y * rect_w, 5, 5, pack_color(255, 255, 255));

	return 0;
}