// Frame.cpp
#include "Frame.hpp"

#include <cstring>
#include <cstdarg>
#include <stdexcept>

#include <glm/common.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Logger.hpp"

#define FORWARD_CB(func, ...) Frame* f = reinterpret_cast<Frame*>(glfwGetWindowUserPointer(window));\
	f->func(__VA_ARGS__);\

namespace ori
{

const char* debug_type_to_string(GLenum debug_type)
{
	switch (debug_type)
	{
	case GL_DEBUG_TYPE_ERROR:
		return "ERROR";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "DEPRECATED";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "UNDEFINED BEHAVIOR";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "PORTABILITY";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "PERFORMANCE";
	case GL_DEBUG_TYPE_OTHER:
		return "OTHER";
	default:
		return "UNKNOWN";
	}
}

void APIENTRY on_opengl_error([[maybe_unused]] GLenum source,
            GLenum type,
			[[maybe_unused]] GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
			[[maybe_unused]] const void *userParam)
{
	auto msg = std::string_view(message, length);

	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		Logger::get().info({"OpenGL {}: {}"}, debug_type_to_string(type), msg);

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		Logger::get().critical({"OpenGL error ({}): {}"}, debug_type_to_string(type), msg);
		Logger::get().dump_backtrace();
		std::abort();
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	case GL_DEBUG_TYPE_PORTABILITY:
	case GL_DEBUG_TYPE_PERFORMANCE:
	case GL_DEBUG_TYPE_OTHER:
		Logger::get().warn({"OpenGL error ({}): {}"}, debug_type_to_string(type), msg);
	}
}

void on_glad_pre_call([[maybe_unused]] const char* name,
		[[maybe_unused]] void* funcptr,
		[[maybe_unused]] int len_args,
		...)
{
	std::vector<int> iargs;

	std::va_list args;
	va_start(args, len_args);
	for (int i = 0; i < len_args && i < 16; ++i)
	{
		iargs.push_back(va_arg(args, int));
	}
	va_end(args);

	Logger::get().trace({"{}({})"}, name, fmt::join(iargs, ", "));
}

//void joystick_cb(int jid, int event);
//void monitor_cb(GLFWmonitor* monitor, int event);
void on_glfw_error(int error_code, const char* description);

void on_key_w(GLFWwindow* window, int key, int scancode, int action, int mods);
void on_char_w(GLFWwindow* window, unsigned int codepoint);
void on_char_mods_w(GLFWwindow* window, unsigned int codepoint, int mods);
void on_mouse_button_w(GLFWwindow* window, int button, int action, int mods);
void on_cursor_w(GLFWwindow* window, double xpos, double ypos);
void on_cursor_enter_w(GLFWwindow* window, int entered);
void on_scroll_w(GLFWwindow* window, double xoffset, double yoffset);
void on_drop_w(GLFWwindow* window, int path_count, const char* paths[]);
void on_window_move_w(GLFWwindow* window, int xpos, int ypos);
void on_window_close_w(GLFWwindow* window);
void on_window_refresh_w(GLFWwindow* window);
void on_window_focus_w(GLFWwindow* window, int focused);
void on_window_iconify_w(GLFWwindow* window, int iconified);
void on_window_maximize_w(GLFWwindow* window, int maximized);
void on_window_resize_w(GLFWwindow* window, int width, int height);
void on_window_content_scale_w(GLFWwindow* window, float xscale, float yscale);

void on_glfw_error([[maybe_unused]] int error_code, const char* description)
{
	Logger::get().error(description);
}

void on_key_w(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, int mods)
{
	FORWARD_CB(on_key, (Key) key, (InputAction) action, (InputModifier) mods);
}

void on_char_w(GLFWwindow* window, unsigned int codepoint)
{
	FORWARD_CB(on_char, codepoint);
}

void on_char_mods_w(GLFWwindow* window, unsigned int codepoint, int mods)
{
	FORWARD_CB(on_char_mods, codepoint, (InputModifier) mods);
}

void on_mouse_button_w(GLFWwindow* window, int button, int action, int mods)
{
	FORWARD_CB(on_mouse_button, (MouseButton) button, (InputAction) action, (InputModifier) mods);
}

void on_cursor_w(GLFWwindow* window, double xpos, double ypos)
{
	FORWARD_CB(on_cursor, glm::vec<2, double>(xpos, ypos));
}

void on_cursor_enter_w(GLFWwindow* window, int entered)
{
	FORWARD_CB(on_cursor_enter, entered);
}

void on_scroll_w(GLFWwindow* window, double xoffset, double yoffset)
{
	FORWARD_CB(on_scroll, glm::vec<2, double>(xoffset, yoffset));
}

void on_drop_w(GLFWwindow* window, int path_count, const char* paths[])
{
	std::vector<std::string_view> vpaths(path_count);

	for (int i = 0; i < path_count; ++i)
	{
		vpaths[i] = std::string_view(paths[i]);
	}

	FORWARD_CB(on_drop, vpaths);
}

void on_window_move_w(GLFWwindow* window, int xpos, int ypos)
{
	FORWARD_CB(on_window_move, glm::ivec2(xpos, ypos));
}

void window_size_cb_w(GLFWwindow* window, int width, int height)
{
	FORWARD_CB(on_window_resize, width, height);
}

void on_window_close_w(GLFWwindow* window)
{
	Frame* f = reinterpret_cast<Frame*>(glfwGetWindowUserPointer(window));
	f->on_window_close();
}

void on_window_refresh_w(GLFWwindow* window)
{
	Frame* f = reinterpret_cast<Frame*>(glfwGetWindowUserPointer(window));
	f->on_window_refresh();
}

void on_window_focus_w(GLFWwindow* window, int focused)
{
	FORWARD_CB(on_window_focus, focused);
}

void on_window_iconify_w(GLFWwindow* window, int iconified)
{
	FORWARD_CB(on_window_iconify, iconified);
}

void on_window_maximize_w(GLFWwindow* window, int maximized)
{
	FORWARD_CB(on_window_maximize, maximized);
}

void on_window_resize_w(GLFWwindow* window, int width, int height)
{
	FORWARD_CB(on_window_resize, width, height);
}

void on_window_content_scale_w(GLFWwindow* window, float xscale, float yscale)
{
	FORWARD_CB(on_window_content_scale, glm::vec2(xscale, yscale));
}

class GLFW
{
public:
	static void inc_ref_count() noexcept
	{
		if (ref_count == 0)
			glfwInit();
		++ref_count;
	}

	static void dec_ref_count() noexcept
	{
		--ref_count;
		if (ref_count == 0)
			glfwTerminate();
	}

private:
	static int ref_count;
};

int GLFW::ref_count = 0;

FrameException::FrameException()
: std::runtime_error("Frame exception")
{
}

Frame::Frame(int width, int height, std::string_view title)
{
	glfwSetErrorCallback(on_glfw_error);

	// init glfw
	GLFW::inc_ref_count();

	// create window
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (!(window && is_open()))
	{
		Logger::get().error("Window creation failed");
		throw FrameException();
	}

	// set callbacks
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, on_key_w);
	glfwSetCharCallback(window, on_char_w);
	glfwSetCharModsCallback(window, on_char_mods_w);
	glfwSetMouseButtonCallback(window, on_mouse_button_w);
	glfwSetCursorPosCallback(window, on_cursor_w);
	glfwSetCursorEnterCallback(window, on_cursor_enter_w);
	glfwSetScrollCallback(window, on_scroll_w);
	glfwSetDropCallback(window, on_drop_w);
	glfwSetWindowPosCallback(window, on_window_move_w);
	glfwSetWindowCloseCallback(window, on_window_close_w);
	glfwSetWindowRefreshCallback(window, on_window_refresh_w);
	glfwSetWindowFocusCallback(window, on_window_focus_w);
	glfwSetWindowIconifyCallback(window, on_window_iconify_w);
	glfwSetWindowMaximizeCallback(window, on_window_maximize_w);
	glfwSetFramebufferSizeCallback(window, on_window_resize_w);
	glfwSetWindowContentScaleCallback(window, on_window_content_scale_w);

	// load opengl pointers
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	int ret = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	if (!ret)
	{
		Logger::get().error("Unable to load OpenGL pointers");
		throw FrameException();
	}

	// set opengl debug callbacks
#ifndef NDEBUG
	glad_set_pre_callback(on_glad_pre_call);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(on_opengl_error, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
}

Frame::Frame(Frame&& other) noexcept
{
	window = other.window;
	other.window = nullptr;
}

Frame::~Frame() noexcept
{
	if (window)
	{
		glfwSetWindowShouldClose(window, true);
		glfwPollEvents();
		glfwDestroyWindow(window);
		GLFW::dec_ref_count();
	}
}

void Frame::on_input()
{
}

void Frame::on_tick([[maybe_unused]] float dt)
{
}

void Frame::on_render([[maybe_unused]] float dt)
{
}

void Frame::run(int tick_hz, int max_frame_hz)
{
	assert(tick_hz > 0);
	assert(max_frame_hz >= 0);

	const double tick = 1.0 / tick_hz;
	const double max_dt = 1.0;
	const double min_frame_time = max_frame_hz ? 1.0 / max_frame_hz : 0.0;

	double previous = time();
	double dt = 0.0;

	while (is_open())
	{
		double current_time = time();
		dt += current_time - previous;
		dt = std::min(dt, max_dt);
		previous = current_time;

		on_input();

		while (dt > tick)
		{
			on_tick(dt);
			dt -= tick;
		}

		on_render(dt);
		swap_buffers();
		while (time() - current_time < min_frame_time);
		update();
	}
}

void Frame::on_key([[maybe_unused]] Key k, [[maybe_unused]] InputAction action, [[maybe_unused]] InputModifier mods)
{
}

void Frame::on_char([[maybe_unused]] unsigned int codepoint)
{
}

void Frame::on_char_mods([[maybe_unused]] unsigned int codepoint, [[maybe_unused]] InputModifier mods)
{
}

void Frame::on_mouse_button([[maybe_unused]] MouseButton mb, [[maybe_unused]] InputAction action, [[maybe_unused]] InputModifier mods)
{
}

void Frame::on_cursor([[maybe_unused]] glm::vec<2, double> position)
{
}

void Frame::on_cursor_enter([[maybe_unused]] bool entered)
{
}

void Frame::on_scroll([[maybe_unused]] glm::vec<2, double> offset)
{
}

void Frame::on_drop([[maybe_unused]] const std::vector<std::string_view>& paths)
{
}

auto Frame::cursor() const noexcept -> glm::vec<2, double>
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return glm::vec<2, double>(xpos, ypos);
}

bool Frame::pressed(Key k) const noexcept
{
	return glfwGetKey(window, static_cast<int>(k)) == GLFW_PRESS;
}

bool Frame::pressed(MouseButton b) const noexcept
{
	return glfwGetMouseButton(window, static_cast<int>(b)) == GLFW_PRESS;
}

bool Frame::released(Key k) const noexcept
{
	return glfwGetKey(window, static_cast<int>(k)) == GLFW_RELEASE;
}

bool Frame::released(MouseButton b) const noexcept
{
	return glfwGetMouseButton(window, static_cast<int>(b)) == GLFW_PRESS;
}

void Frame::on_window_move([[maybe_unused]] glm::ivec2 position)
{
}

void Frame::on_window_close()
{
}

void Frame::on_window_refresh()
{
}

void Frame::on_window_focus([[maybe_unused]] bool focused)
{
}

void Frame::on_window_iconify([[maybe_unused]] bool iconified)
{
}

void Frame::on_window_maximize([[maybe_unused]] bool maximized)
{
}

void Frame::on_window_resize([[maybe_unused]] int width, [[maybe_unused]] int height)
{
}

void Frame::on_window_content_scale([[maybe_unused]] glm::vec2 scale)
{
}

void Frame::update() noexcept
{
	glfwPollEvents();
}

void Frame::swap_buffers() noexcept
{
	if (is_open())
		glfwSwapBuffers(window);
}

void Frame::close() noexcept
{
	glfwSetWindowShouldClose(window, true);
}

void Frame::set_cursor_locked(bool locked) noexcept
{
	if (locked)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Frame::set_time(double time) noexcept
{
	glfwSetTime(time);
}

void Frame::set_icon(unsigned char* data, int width, int height) noexcept
{
	GLFWimage image;
	image.width = width;
	image.height = height;
	image.pixels = data;
	glfwSetWindowIcon(window, 1, &image);
}

bool Frame::is_open() const noexcept
{
	if (window == nullptr)
		return false;

	return glfwWindowShouldClose(window) == false;
}

auto Frame::width() const noexcept -> int
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}

auto Frame::height() const noexcept -> int
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

auto Frame::time() const noexcept -> double
{
	return glfwGetTime();
}

}
