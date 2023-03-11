#include "klib.h"


static constexpr int sphere_count = 10;

struct colored_sphere
{
    kl::float3 center = {};
    float radius = 0.0f;
    kl::float4 color = {};
};

struct ps_cb
{
    kl::float4 frame_size = {};
    kl::float4x4 inverse_camera = {};
    kl::float4 camera_position = {};
    kl::float4 sun_direction = {};
    colored_sphere spheres[sphere_count] = {};
};

int main()
{
    kl::timer timer = {};
    kl::camera camera = {};

    kl::window window = { { 1600, 900 }, "Raytracing" };
    kl::gpu gpu = { (HWND) window };

    // Heap alloc because of stack size warnings
    ps_cb& ps_data = *new ps_cb; // You saw nothing :)

    window.on_resize.push_back([&](const kl::int2 new_size)
    {
        if (new_size.x > 0 && new_size.y > 0) {
            gpu.resize_internal(new_size);
            gpu.set_viewport_size(new_size);
            camera.update_aspect_ratio(new_size);
        }
    });

    window.keyboard.r.on_press.push_back([&]
    {
        if (window.keyboard.shift) {
            for (auto& [center, radius, color] : ps_data.spheres) {
                color = kl::float4(kl::random::get_color());
            }
        }
        else if (window.keyboard.ctrl) {
            for (auto& [center, radius, color] : ps_data.spheres) {
                color = kl::float4(kl::color(color).gray());
            }
        }
        else {
            for (auto& sphere : ps_data.spheres) {
                sphere = colored_sphere{
                    kl::random::get_float3(40.0f) - kl::float3(20.0f, 20.0f, 20.0f),
                    kl::random::get_float(2.75f) + 0.25f,
                    kl::float4(kl::random::get_color())
                };
            }
        }
    });

    window.mouse.right.on_down.push_back([&]
    {
        const kl::ray ray = { camera.position, kl::math::inverse(camera.matrix()), window.mouse.get_normalized_position()};
        ps_data.sun_direction = { ray.direction * -1.0f, 0.0f };
    });

    // Start
    window.maximize();

    const std::string shader_sources = kl::files::read_string("examples/shaders/raytracing.hlsl");
    const auto shaders = gpu.create_render_shaders(shader_sources);
    gpu.bind_render_shaders(shaders);

    const auto screen_mesh = gpu.create_screen_mesh();

    auto pixel_const_buffer = gpu.create_const_buffer(sizeof(ps_cb));
    gpu.bind_cb_for_pixel_shader(pixel_const_buffer, 0);

    camera.position.y = 5.0f;
    ps_data.sun_direction = { kl::math::normalize(kl::float3(-1.0f, -1.0f, 0.0f)), 0.0f };

    window.keyboard.r.on_press.back()();

    // Update
    while (window.process(false)) {
        timer.update_interval();

        { // Physics
            for (int i = 0; i < sphere_count; i++) {
                const float oscillation = (std::sin(timer.get_elapsed() + i) + 1.0f) * 0.5f;
                ps_data.spheres[i].center.y = (oscillation * (i + 1.0f)) + ps_data.spheres[i].radius;
            }
        }

        { // Input
            static bool camera_rotating = false;
            if (window.mouse.left) {
                const kl::int2 frame_center = window.get_frame_center();

                if (camera_rotating) {
                    camera.rotate(kl::float2(window.mouse.position()), kl::float2(frame_center));
                }
                window.mouse.set_position(frame_center);

                window.mouse.set_hidden(true);
                camera_rotating = true;
            }
            else if (camera_rotating) {
                window.mouse.set_hidden(false);
                camera_rotating = false;
            }

            if (window.keyboard.w) {
                camera.move_forward(timer.get_interval());
            }
            if (window.keyboard.s) {
                camera.move_back(timer.get_interval());
            }
            if (window.keyboard.d) {
                camera.move_right(timer.get_interval());
            }
            if (window.keyboard.a) {
                camera.move_left(timer.get_interval());
            }
            if (window.keyboard.e) {
                camera.move_up(timer.get_interval());
            }
            if (window.keyboard.q) {
                camera.move_down(timer.get_interval());
            }
        }

        // Render
        gpu.clear_internal();

        ps_data.frame_size = { kl::float2(window.size()), {} };
        ps_data.inverse_camera = kl::math::inverse(camera.matrix());
        ps_data.camera_position = { camera.position, 0.0f };
        gpu.set_cb_data(pixel_const_buffer, ps_data);

        gpu.draw_mesh(screen_mesh);

        gpu.swap_buffers(true);

        window.set_title(kl::format(int(1.0f / timer.get_interval())));
    }
}