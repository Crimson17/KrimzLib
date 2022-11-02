#include "klib.h"


struct stick
{
	uint32_t value = 0;
	kl::color color = {};
};

static std::vector<stick> generate_sticks(const uint32_t count, uint32_t min_value_incl, uint32_t max_value_excl)
{
	uint32_t stored_min_value = max_value_excl;
	uint32_t stored_max_value = min_value_incl;
	
	std::vector<stick> sticks(count);
	for (auto& [value, color] : sticks) {
		value = kl::random::get_int(static_cast<int>(min_value_incl), static_cast<int>(max_value_excl));
		stored_min_value = std::min(stored_min_value, value);
		stored_max_value = std::max(stored_max_value, value);
	}
	
	for (auto& [value, color] : sticks) {
		const auto gray_value = static_cast<uint8_t>(kl::math::normalize(value, stored_min_value, stored_max_value) * 255);
		color = { gray_value, gray_value, gray_value };
	}
	
	return sticks;
}

static void draw_sticks(kl::image& frame, const std::vector<stick>& sticks)
{
	for (int i = 0; i < static_cast<int>(sticks.size()); i++) {
		frame.draw_line({ i, frame.height() - 1 }, { i, frame.height() - 1 - sticks[i].value }, sticks[i].color);
	}
}

int main()
{
	kl::window window = { { 1600, 900 }, "Visual Sort" };
	auto frame = kl::image(window.size(), kl::colors::gray);

	std::vector<stick> sticks = generate_sticks(frame.width(), 1, frame.height());

	std::thread([&] {
		window.set_title("Sorting...");
		
		for (int i = 0; i < static_cast<int>(sticks.size()) - 1; i++) {
			for (int j = i + 1; j < static_cast<int>(sticks.size()); j++) {
				if (sticks[j].value < sticks[i].value) {
					std::swap(sticks[i], sticks[j]);
				}
				kl::time::wait(0.000005f);
			}
		}
		
		window.set_title("Finished!");
	}).detach();

	while (window.process(false)) {
		frame.fill(kl::colors::gray);
		draw_sticks(frame, sticks);
		window.draw_image(frame);
	}
}