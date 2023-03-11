#include "klib.h"


struct vs_cb
{
    kl::float4x4 w_matrix;
    kl::float4x4 vp_matrix;
    kl::float4 misc_data;
};

struct ps_cb
{
    kl::float4 object_color;
    kl::float4 sun_direction;
};

int main()
{
    /* ----- SETUP ----- */
    kl::window window = { { 1600, 900 }, "Geometry Test" };
    kl::gpu gpu = { (HWND) window };

    kl::timer timer = {};
    kl::camera camera = {};

    kl::float3 sun_direction = { 1.0f, -1.0f, 0.0f };

    window.on_resize.push_back([&](const kl::int2 new_size)
    {
        if (new_size.x > 0 && new_size.y > 0) {
            gpu.resize_internal(new_size);
            gpu.set_viewport_size(new_size);
            camera.update_aspect_ratio(new_size);
        }
    });
    window.maximize();

    window.keyboard.v.on_press.push_back([&]
    {
        static bool wireframe_bound = true;
        static auto solid_raster = gpu.create_raster_state(false, false);
        static auto wireframe_raster = gpu.create_raster_state(true, false);

        gpu.bind_raster_state(wireframe_bound ? solid_raster : wireframe_raster);
        wireframe_bound = !wireframe_bound;
    });
    window.keyboard.v.on_press.back()();

    std::string shader_sources = kl::files::read_string("examples/shaders/geometry_test.hlsl");
    auto default_shaders = gpu.create_render_shaders(shader_sources);
    auto geometry_shader = gpu.create_geometry_shader(shader_sources);
    gpu.bind_render_shaders(default_shaders);
    gpu.bind_geometry_shader(geometry_shader);

    auto vs_cbuffer = gpu.create_const_buffer(sizeof(vs_cb));
    auto ps_cbuffer = gpu.create_const_buffer(sizeof(ps_cb));

    gpu.bind_cb_for_vertex_shader(vs_cbuffer, 0);
    gpu.bind_cb_for_pixel_shader(ps_cbuffer, 0);

    auto cube_mesh = gpu.create_mesh("examples/meshes/cube.obj", true);
    auto sphere_mesh = gpu.create_mesh("examples/meshes/sphere.obj", true);
    auto monke_mesh = gpu.create_mesh("examples/meshes/monke.obj", true);

    kl::ref<kl::entity> main_entity = kl::make<kl::entity>();
    main_entity->angular.y = -36.0f;
    main_entity->mesh = monke_mesh;
    main_entity->material->color = kl::colors::orange;

    window.keyboard.num1.on_press.push_back([&]
    {
        main_entity->mesh = cube_mesh;
    });
    window.keyboard.num2.on_press.push_back([&]
    {
        main_entity->mesh = sphere_mesh;
    });
    window.keyboard.num3.on_press.push_back([&]
    {
        main_entity->mesh = monke_mesh;
    });

    float destroy_goal = 0.0f;
    float destroy_value = 0.0f;
    window.keyboard.space.on_press.push_back([&]
    {
        destroy_goal = 1.5f;
    });

    camera.position = { -2.0f, 2.0f, -2.0f };
    camera.set_forward(camera.position * -1.0f);

    /* ----- UPDATE ----- */
    while (window.process(false)) {
        timer.update_interval();

        main_entity->update_physics(timer.get_interval());

        if (destroy_value < destroy_goal) {
            destroy_value += timer.get_interval() * 5.0f;
        }
        else {
            destroy_goal = -INFINITY;
            destroy_value -= timer.get_interval() * 0.25f;
        }

        sun_direction = kl::math::normalize(sun_direction);

        gpu.clear_internal(kl::colors::gray);

        vs_cb vs_data = {};
        vs_data.vp_matrix = camera.matrix();
        vs_data.w_matrix = main_entity->matrix();
        vs_data.misc_data.x = max(destroy_value, 0.0f);
        gpu.set_cb_data(vs_cbuffer, vs_data);

        ps_cb ps_data = {};
        ps_data.sun_direction = { sun_direction.x, sun_direction.y, sun_direction.z, 0.0f };
        ps_data.object_color = main_entity->material->color;
        gpu.set_cb_data(ps_cbuffer, ps_data);

        if (main_entity->mesh) {
            gpu.draw_mesh(main_entity->mesh);
        }

        gpu.swap_buffers(true);
    }
}