#include "geometry/sphere.h"


kl::sphere::sphere() {}
kl::sphere::sphere(const kl::float3& center, float radius, const kl::float3& color, float reflectivity, float emission) : center(center), radius(radius), color(color), reflectivity(reflectivity), emission(emission) {}

// Emissive color
kl::float3 kl::sphere::emiss() const {
	return color * emission;
}

// std::cout
std::ostream& kl::operator<<(std::ostream& os, const kl::sphere& obj) {
	os << "{" << obj.center << ", " << obj.radius << "}";
	return os;
}
