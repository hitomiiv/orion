// Rect2D.hpp
#ifndef RECT_HPP_
#define RECT_HPP_

#include <algorithm>
#include <tuple>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace ori
{

template <class T>
struct Rect1D_base
{
	using value_type = T;
	value_type size = value_type(1);
	value_type offset = value_type(0);
};

template <class T>
struct Rect2D_base
{
	using value_type = T;
	using vec_type = glm::vec<2, value_type>;

	Rect2D_base() = default;

	Rect2D_base(vec_type position, value_type width, value_type height)
	: position(position), width(width), height(height)
	{
	}

	vec_type position = vec_type(0);
	value_type width = value_type(1);
	value_type height = value_type(1);
};

using Rect1D = Rect1D_base<float>;
using Rect2D = Rect2D_base<float>;
using iRect1D = Rect1D_base<std::size_t>;
using iRect2D = Rect2D_base<std::size_t>;

template <class T>
auto overlap_calc_x(const Rect2D_base<T>& left, const Rect2D_base<T>& right)
-> typename Rect2D_base<T>::value_type
{
	return std::min(Rect2D_base<T>::value_type(0), left_edge(right) - right_edge(left));
}

template <class T>
auto overlap_calc_y(const Rect2D_base<T>& bottom, const Rect2D_base<T>& top)
-> typename Rect2D_base<T>::value_type
{
	return std::min(Rect2D_base<T>::value_type(0), bottom_edge(top) - top_edge(bottom));
}

template <class T>
auto collision_offset(const Rect2D_base<T>& a, const Rect2D_base<T>& b)
-> typename Rect2D_base<T>::vec_type
{
	typename Rect2D_base<T>::vec_type v;

	if (a.position.x < b.position.x)
		v.position.x = overlap_calc_x(a, b);
	else
		v.position.x = -overlap_calc_x(b, a);

	if (a.position.y < b.position.y)
		v.position.y = overlap_calc_y(a, b);
	else
		v.position.y = -overlap_calc_y(b, a);

	return v;
}

template <class T>
auto containment_offset(const Rect2D_base<T>& outer, const Rect2D_base<T>& inner)
-> typename Rect2D_base<T>::vec_type
{
	using vec_type = typename Rect2D_base<T>::vec_type;
	using value_type = typename Rect2D_base<T>::value_type;

	vec_type v = {};
	value_type dist;

	dist = left_edge(inner) - left_edge(outer);
	if (dist < value_type(0))
		v.x = dist;

	dist = right_edge(outer) - right_edge(inner);
	if (dist < value_type(0))
		v.x = -dist;

	dist = bottom_edge(inner) - bottom_edge(outer);
	if (dist < value_type(0))
		v.y = dist;

	dist = top_edge(outer) - top_edge(inner);
	if (dist < value_type(0))
		v.y = -dist;

	return v;
}

template <class T>
auto collides(const Rect2D_base<T>& a, const Rect2D_base<T>& b)
-> bool
{
	return collides(collision_offset(a, b));
}

template <class T>
auto collides(typename Rect2D_base<T>::vec_type col)
-> bool
{
	return col.position != typename Rect2D_base<T>::vec_type(0);
}

template <class T>
auto contains(const Rect2D_base<T>& outer, const Rect2D_base<T>& inner)
-> bool
{
	return contains(containment_offset(outer, inner));
}

template <class T>
auto contains(typename Rect2D_base<T>::vec_type col)
-> bool
{
	return col.position == typename Rect2D_base<T>::vec_type(0);
}

template <class T>
auto top_edge(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::value_type
{
	return r.position.y + r.height;
}

template <class T>
auto bottom_edge(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::value_type
{
	return r.position.y;
}

template <class T>
auto right_edge(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::value_type
{
	return r.position.x + r.width;
}

template <class T>
auto left_edge(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::value_type
{
	return r.position.x;
}

template <class T>
auto top_left(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::vec_type
{
	return typename Rect2D_base<T>::vec_type(r.position.x, r.position.y + r.height);
}

template <class T>
auto top_right(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::vec_type
{
	return typename Rect2D_base<T>::vec_type(r.position.x + r.width, r.position.y + r.height);
}

template <class T>
auto bottom_left(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::vec_type
{
	return r.position;
}

template <class T>
auto bottom_right(const Rect2D_base<T>& r) noexcept
-> typename Rect2D_base<T>::vec_type
{
	return typename Rect2D_base<T>::vec_type(r.position.x + r.width, r.position.y);
}

template <class T>
auto center(Rect2D_base<T> r, typename Rect2D_base<T>::vec_type pos)
{
	auto dim = glm::vec2(r.width, r.height);
	r.position = pos - dim / T{2};
	return r;
}

template <class T>
bool operator ==(const Rect2D_base<T>& a, const Rect2D_base<T>& b) noexcept
{
	return std::tie(a.position, a.width, a.height)
			== std::tie(b.position, b.width, b.height);
}

template <class T>
bool operator !=(const Rect2D_base<T>& a, const Rect2D_base<T>& b) noexcept
{
	return !(a == b);
}

auto model(const Rect2D& r) noexcept -> glm::mat4;
auto view(const Rect2D& r) noexcept -> glm::mat4;
auto projection(const Rect2D& r) noexcept -> glm::mat4;

}

#endif // RECT_HPP_
