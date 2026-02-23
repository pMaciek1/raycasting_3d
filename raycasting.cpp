#define _USE_MATH_DEFINES
#include <cmath>
#include <iomanip>
#include <sstream>

#include "map.h"
#include "utils.h"
#include "framebuffer.h"
#include "player.h"

void render(FrameBuffer& fb, Map& map, Player& player) {
	fb.clear(pack_color(255, 255, 255));

	const size_t rect_w = fb.w / (map.w * 2);
	const size_t rect_h = fb.h / map.h;
	for (size_t j = 0; j < map.h;j++) {
		for (size_t i = 0;i < map.w;i++) {
			if (map.is_empty(i, j)) continue;
			size_t rect_x = i * rect_w;
			size_t rect_y = j * rect_h;
			fb.draw_rect(rect_x, rect_y, rect_w, rect_h, pack_color(255, 0, 255));
		}
	}
	for (size_t i = 0; i < fb.w / 2; i++) {
		float angle = player.dr - player.fov / 2 + player.fov * i / float(fb.w / 2);
		for (float c = 0; c < 22.6; c += 0.1) {
			float x = player.x + c * cos(angle);
			float y = player.y + c * sin(angle);
			fb.set_pixel(x * rect_w, y * rect_h, pack_color(128, 128, 128));

			if (map.is_empty(x, y)) continue;
			size_t column_h = fb.h / (c * cos(angle - player.dr));
			int pix_x = i + fb.w / 2;
			for (size_t j = 0; j < column_h; j++) {
				int pix_y = j + fb.h / 2 - column_h / 2;
				if (pix_y >= 0 && pix_y < (int)fb.h) {
					fb.set_pixel(pix_x, pix_y, pack_color(255, 0, 255));
				}
			}
			break;
		}
	}
}

int main() {

	FrameBuffer fb{ 1024, 512, std::vector<uint32_t>(1024 * 512, pack_color(255,255,255)) };
	Player player{ 3.456, 2.345, 1.523, M_PI / 3. };
	Map map;

	for (size_t frame = 0; frame < 360; frame++) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
		player.dr += 2 * M_PI / 360;
		render(fb, map, player);
		drop_ppm_image(ss.str(), fb.img, fb.w, fb.h);
	}

	return 0;
}