// Frame.hpp
#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <glm/vec2.hpp>

extern "C" typedef struct GLFWwindow GLFWwindow;

namespace ori
{

enum class Key : int
{
	unknown            = -1,
	space              = 32,
	apostrophe         = 39,
	comma              = 44,
	minus              = 45,
	period             = 46,
	slash              = 47,
	_0                 = 48,
	_1                 = 49,
	_2                 = 50,
	_3                 = 51,
	_4                 = 52,
	_5                 = 53,
	_6                 = 54,
	_7                 = 55,
	_8                 = 56,
	_9                 = 57,
	semicolon          = 59,
	equal              = 61,
	a                  = 65,
	b                  = 66,
	c                  = 67,
	d                  = 68,
	e                  = 69,
	f                  = 70,
	g                  = 71,
	h                  = 72,
	i                  = 73,
	j                  = 74,
	k                  = 75,
	l                  = 76,
	m                  = 77,
	n                  = 78,
	o                  = 79,
	p                  = 80,
	q                  = 81,
	r                  = 82,
	s                  = 83,
	t                  = 84,
	u                  = 85,
	v                  = 86,
	w                  = 87,
	x                  = 88,
	y                  = 89,
	z                  = 90,
	left_bracket       = 91,
	backslash          = 92,
	right_bracket      = 93,
	grave_accent       = 96,
	world_1            = 161,
	world_2            = 162,
	escape             = 256,
	enter              = 257,
	tab                = 258,
	backspace          = 259,
	insert             = 260,
	_delete            = 261,
	right              = 262,
	left               = 263,
	down               = 264,
	up                 = 265,
	page_up            = 266,
	page_down          = 267,
	home               = 268,
	end                = 269,
	caps_lock          = 280,
	scroll_lock        = 281,
	num_lock           = 282,
	print_screen       = 283,
	pause              = 284,
	f1                 = 290,
	f2                 = 291,
	f3                 = 292,
	f4                 = 293,
	f5                 = 294,
	f6                 = 295,
	f7                 = 296,
	f8                 = 297,
	f9                 = 298,
	f10                = 299,
	f11                = 300,
	f12                = 301,
	f13                = 302,
	f14                = 303,
	f15                = 304,
	f16                = 305,
	f17                = 306,
	f18                = 307,
	f19                = 308,
	f20                = 309,
	f21                = 310,
	f22                = 311,
	f23                = 312,
	f24                = 313,
	f25                = 314,
	kp_0               = 320,
	kp_1               = 321,
	kp_2               = 322,
	kp_3               = 323,
	kp_4               = 324,
	kp_5               = 325,
	kp_6               = 326,
	kp_7               = 327,
	kp_8               = 328,
	kp_9               = 329,
	kp_decimal         = 330,
	kp_divide          = 331,
	kp_multiply        = 332,
	kp_subtract        = 333,
	kp_add             = 334,
	kp_enter           = 335,
	kp_equal           = 336,
	left_shift         = 340,
	left_control       = 341,
	left_alt           = 342,
	left_super         = 343,
	right_shift        = 344,
	right_control      = 345,
	right_alt          = 346,
	right_super        = 347,
	menu               = 348,
};

enum class MouseButton : int
{
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	left   = _1,
	right  = _2,
	middle = _3,
};

enum class InputAction : int
{
	released,
	pressed,
	repeated,
};

enum class InputModifier : int
{
	shift		= 0x01,
	control		= 0x02,
	alt			= 0x04,
	super		= 0x08,
	caps_lock	= 0x10,
	num_lock	= 0x20,
};

constexpr auto operator|(InputModifier a, InputModifier b) noexcept
{
	return static_cast<InputModifier>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr auto operator&(InputModifier a, InputModifier b) noexcept
{
	return static_cast<InputModifier>(static_cast<int>(a) & static_cast<int>(b));
}

constexpr auto operator~(InputModifier a) noexcept
{
	return static_cast<InputModifier>(~static_cast<int>(a));
}

constexpr auto operator|=(InputModifier& a, InputModifier b) noexcept
{
	return a = (a | b);
}

constexpr auto operator&=(InputModifier& a, InputModifier b) noexcept
{
	return a = (a & b);
}

class FrameException : public std::runtime_error
{
public:
	FrameException();
};

class Frame
{
public:
	Frame() = default;
	Frame(int width, int height, std::string_view title);
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) noexcept;
	Frame& operator=(const Frame& other) = delete;
	Frame& operator=(Frame&& other) = delete;
	virtual ~Frame() noexcept;

	// main loop
	virtual void on_input();
	virtual void on_tick(float dt);
	virtual void on_render(float dt);
	virtual void run(int ticks_per_second, int fps_cap);

	// input events
	virtual void on_key(Key k, InputAction action, InputModifier mods);
	virtual void on_char(unsigned int codepoint);
	virtual void on_char_mods(unsigned int codepoint, InputModifier mods);
	virtual void on_mouse_button(MouseButton mb, InputAction action, InputModifier mods);
	virtual void on_cursor(glm::vec<2, double> position);
	virtual void on_cursor_enter(bool entered);
	virtual void on_scroll(glm::vec<2, double> offset);
	virtual void on_drop(const std::vector<std::string_view>& paths);

	// input polling
	auto cursor() const noexcept -> glm::vec<2, double>;
	bool pressed(Key k) const noexcept;
	bool pressed(MouseButton mb) const noexcept;
	bool released(Key k) const noexcept;
	bool released(MouseButton mb) const noexcept;

	// window events
	virtual void on_window_move(glm::ivec2 position);
	virtual void on_window_resize(int width, int height);
	virtual void on_window_close();
	virtual void on_window_refresh();
	virtual void on_window_focus(bool focused);
	virtual void on_window_iconify(bool iconified);
	virtual void on_window_maximize(bool maximized);
	virtual void on_window_content_scale(glm::vec2 scale);

	// state management
	void swap_buffers() noexcept;
	void update() noexcept;
	void close() noexcept;
	void set_cursor_locked(bool locked) noexcept;
	void set_time(double time) noexcept;
	void set_icon(unsigned char* data, int width, int height) noexcept;

	bool is_open() const noexcept;
	auto width() const noexcept -> int;
	auto height() const noexcept -> int;
	auto time() const noexcept -> double;

private:
	GLFWwindow* window = nullptr;
};

}


#endif // FRAME_HPP_
