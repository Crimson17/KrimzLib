#include "klib.h"


static void draw_axis(kl::image& frame, const kl::color& color = { 75, 75, 75 })
{
    frame.draw_line(kl::int2(0, frame.height() / 2), { frame.width(), frame.height() / 2 }, color);
    frame.draw_line(kl::int2(frame.width() / 2, 0), { frame.width() / 2, frame.height() }, color);
}

static void draw_data(kl::image& frame, const std::vector<kl::int2>& data, const kl::color& color = kl::colors::orange)
{
    const kl::int2 half_size = frame.size() / 2;
    for (auto& val : data) {
        const kl::int2 write_position = val * kl::int2(1, -1) + half_size;
        if (frame.in_bounds(write_position)) {
            frame[write_position] = color;
        }
    }
}

static void draw_line(kl::image& frame, const kl::float2& equat, const kl::color& color = kl::colors::sky)
{
    const kl::int2 half_size = frame.size() / 2;
    const kl::int2 pos1 = kl::int2(-half_size.x, (int) (-half_size.x * equat.x + equat.y)) * kl::int2(1, -1) + half_size;
    const kl::int2 pos2 = kl::int2(half_size.x, (int) (half_size.x * equat.x + equat.y)) * kl::int2(1, -1) + half_size;
    frame.draw_line(pos1, pos2, color);
}

static float calculate_offsets(const std::vector<kl::int2>& data, const kl::float2& line_equat)
{
    float sum = 0.0f;
    for (auto& val : data) {
        sum += abs(val.y - (val.x * line_equat.x + line_equat.y));
    }
    return sum;
}

static void draw_offset(kl::image& frame, const std::vector<kl::int2>& data, const kl::float2& lineEquat, const kl::color& color = kl::colors::yellow)
{
    if (!data.empty()) {
        static size_t data_index = 0;

        data_index = (data_index + 1) % data.size();
        const kl::int2 half_size = frame.size() / 2;
        const kl::int2 pos1 = kl::int2(data[data_index].x, data[data_index].y) * kl::int2(1, -1) + half_size;
        const kl::int2 pos2 = kl::int2(data[data_index].x, (int) (data[data_index].x * lineEquat.x + lineEquat.y)) * kl::int2(1, -1) + half_size;
        frame.draw_line(pos1, pos2, color);
    }
}

static void calculate_improved_line(const std::vector<kl::int2>& data, kl::float2& line_equat)
{
    static kl::float2 alter_x(10.0f, 0.0f);
    static kl::float2 alter_y(0.0f, 10.0f);

    if (static uint64_t last_data_size = 0; data.size() != last_data_size) {
        last_data_size = data.size();
        alter_x = { 10.0f, 0.0f };
        alter_y = { 0.0f, 10.0f };
        line_equat = { 1.0f, 0.0f };
    }

    const float orig_sum = calculate_offsets(data, line_equat);

    if (calculate_offsets(data, line_equat + alter_x) < orig_sum) {
        line_equat += alter_x;
    }
    else if (calculate_offsets(data, line_equat - alter_x) < orig_sum) {
        line_equat -= alter_x;
    }
    else {
        alter_x *= 0.75f;
    }

    if (calculate_offsets(data, line_equat + alter_y) < orig_sum) {
        line_equat += alter_y;
    }
    else if (calculate_offsets(data, line_equat - alter_y) < orig_sum) {
        line_equat -= alter_y;
    }
    else {
        alter_y *= 0.75f;
    }
}

int main()
{
    auto window = kl::window({ 1600, 900 }, "Data Line");
    kl::image frame = kl::image(window.size());

    std::vector<kl::int2> data = {};
    kl::float2 line_equat(1.0f, 0.0f);

    window.mouse.left.on_down.push_back([&]
    {
        static kl::int2 last_data = {};
        kl::int2 new_data = window.mouse.position() * kl::int2(1, -1);
        new_data -= frame.size() / kl::int2(2, -2);
        if (new_data != last_data) {
            data.push_back(new_data);
            last_data = new_data;
        }
    });

    window.keyboard.r.on_press.push_back([&]
    {
        data.clear();
    });

    while (window.process(false)) {
        frame.fill(kl::colors::gray);

        draw_axis(frame);
        draw_data(frame, data);
        draw_line(frame, line_equat);
        draw_offset(frame, data, line_equat);

        calculate_improved_line(data, line_equat);

        window.draw_image(frame);
    }
}