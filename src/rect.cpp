// Rect2D.cpp
#include "rect.hpp"

#include <glm/gtx/transform.hpp>

namespace ori
{

auto model(const Rect2D& r) noexcept -> glm::mat4
{
	auto scale_vec = glm::vec3(r.width, r.height, 0);
	auto translate_vec = glm::vec3(r.position.x, r.position.y, 0);
	auto scaled = glm::scale(glm::mat4(1), scale_vec);
	return glm::translate(scaled, translate_vec);
}

auto view([[maybe_unused]] const Rect2D& r) noexcept -> glm::mat4
{
	return glm::mat4(1);
}

auto projection(const Rect2D& r) noexcept -> glm::mat4
{
	return glm::ortho(left_edge(r), right_edge(r), bottom_edge(r), top_edge(r));
}

}
