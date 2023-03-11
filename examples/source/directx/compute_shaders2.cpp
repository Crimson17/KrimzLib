#include "klib.h"


struct example_struct
{
    kl::float3 position;
};

int main()
{
    kl::gpu gpu = {};

    // CPU buffer
    static constexpr int data_count = 10;
    example_struct example_data[data_count] = {};

    // GPU buffer
    auto buffer = gpu.create_structured_buffer(example_data, data_count, sizeof(example_struct), true, true);
    auto access_view = gpu.create_access_view(buffer, nullptr);
    gpu.bind_access_view_for_compute_shader(access_view, 0);

    // Compiled shader
    const std::string shader_source = kl::files::read_string("examples/shaders/compute_test2.hlsl");
    auto compute_shader = gpu.create_compute_shader(shader_source);

    // Run shader
    gpu.bind_compute_shader(compute_shader);
    gpu.dispatch_compute_shader(data_count, 1, 1);

    // Copy from GPU to CPU
    gpu.read_from_resource(example_data, buffer, (data_count * sizeof(example_struct)));

    // Print values
    for (auto& [position] : example_data) {
        print(position);
    }


    kl::get();
}