// utils.hpp
#ifndef WRAPPERS_HPP_
#define WRAPPERS_HPP_

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <gsl/span>

#include "rect.hpp"

struct __GLsync;
typedef __GLsync* GLsync;

namespace ori
{

namespace detail
{

using swallow_t = int[];

template <int D, class T, glm::qualifier Q>
constexpr std::integral_constant<int, D> vec_dimensions_impl(glm::vec<D, T, Q>) noexcept
{
	return {};
}

template <class T>
constexpr int vec_dimensions() noexcept
{
	return decltype(detail::vec_dimensions_impl(std::declval<T>()))::value;
}

template <std::size_t I, class T>
constexpr std::size_t tuple_element_offset() noexcept
{
	return static_cast<std::size_t>((char*) &std::get<I>(*(T*) 0) - (char*) 0);
}

} // namespace detail

#define TUPLE_TYPE(index, tuple_type) std::tuple_element_t<index, tuple_type>
#define TUPLE_OFFSET(index, tuple_type) detail::tuple_element_offset<index, tuple_type>()
#define TUPLE_FOR_EACH(expr) (void) detail::swallow_t {((expr), 0)...}

/*
 * External, application-side image formats
 */
enum class ImageFormat : std::uint32_t
{
	deduce = 0,
	r      = 0x1903,
	rg     = 0x8227,
	rgb    = 0x1907,
	rgba   = 0x1908,
	bgr    = 0x80E0,
	bgra   = 0x80E1,
};

/*
 * Internal gpu-side image formats
 */
enum class InternalFormat : std::uint32_t
{
	r_8      = 0x8229,
	rg_8     = 0x822B,
	rgb_8    = 0x8051,
	rgba_8   = 0x8058,
	srgb_8   = 0x8C41,
	srgba_8  = 0x8C43,

	r_16f    = 0x822D,
	rg_16f   = 0x822F,
	rgb_16f  = 0x881B,
	rgba_16f = 0x881A,

	r_32f    = 0x822E,
	rg_32f   = 0x8230,
	rgb_32f  = 0x8815,
	rgba_32f = 0x8814,

	depth_component = 0x1902,
};

/*
 * Types of attachments for a Framebuffer
 */
enum class FramebufferAttachment
{
	color         = 0x8CE0,
	depth         = 0x8D00,
	depth_stencil = 0x821A,
};

class ImageException : public std::runtime_error
{
public:
	ImageException();
};

struct Image;
struct ImageDeleter;

using ImagePtr = std::unique_ptr<Image, ImageDeleter>;

/*
 * 2D image
 */
struct Image
{
	static auto load(std::string_view path,
					 ImageFormat format = ImageFormat::deduce,
					 bool flip_vertically = true)
	-> ImagePtr;

	unsigned char* data = nullptr;
	int width = 0;
	int height = 0;
	ImageFormat format = ImageFormat::rgba;
};

/*
 * Deleter for Images loaded from a file
 */
struct ImageDeleter
{
	void operator()(Image* image) const noexcept;
};

/*
 * Base class for OpenGL object handles
 */
class ResourceHandle
{
public:
	ResourceHandle() = default;
	explicit ResourceHandle(std::uint32_t __id) noexcept;
	ResourceHandle(ResourceHandle&& other) noexcept;

	// deleted copy + assign
	ResourceHandle(const ResourceHandle& other) = delete;
	ResourceHandle& operator=(const ResourceHandle& other) = delete;
	ResourceHandle& operator=(ResourceHandle&& other) = delete;

	auto id() const noexcept -> std::uint32_t;
	bool operator ==(const ResourceHandle& other) const noexcept;
	bool operator !=(const ResourceHandle& other) const noexcept;

protected:
	std::uint32_t _id = 0;
};

class VertexShaderHandle : public ResourceHandle
{
public:
	VertexShaderHandle();
	VertexShaderHandle(VertexShaderHandle&&) = default;
	~VertexShaderHandle();
};

class FragmentShaderHandle : public ResourceHandle
{
public:
	FragmentShaderHandle();
	FragmentShaderHandle(FragmentShaderHandle&&) = default;
	~FragmentShaderHandle();
};

class GeometryShaderHandle : public ResourceHandle
{
public:
	GeometryShaderHandle();
	GeometryShaderHandle(GeometryShaderHandle&&) = default;
	~GeometryShaderHandle();
};

class TessControlShaderHandle : public ResourceHandle
{
public:
	TessControlShaderHandle();
	TessControlShaderHandle(TessControlShaderHandle&&) = default;
	~TessControlShaderHandle();
};

class TessEvaluationShaderHandle : public ResourceHandle
{
public:
	TessEvaluationShaderHandle();
	TessEvaluationShaderHandle(TessEvaluationShaderHandle&&) = default;
	~TessEvaluationShaderHandle();
};

class ComputeShaderHandle : public ResourceHandle
{
public:
	ComputeShaderHandle();
	ComputeShaderHandle(ComputeShaderHandle&&) = default;
	~ComputeShaderHandle();
};

class ShaderProgramHandle : public ResourceHandle
{
public:
	ShaderProgramHandle();
	ShaderProgramHandle(ShaderProgramHandle&&) = default;
	~ShaderProgramHandle();
};

class BufferHandle : public ResourceHandle
{
public:
	BufferHandle();
	BufferHandle(BufferHandle&&) = default;
	~BufferHandle();
};

class Texture2DHandle : public ResourceHandle
{
public:
	Texture2DHandle();
	Texture2DHandle(Texture2DHandle&&) = default;
	~Texture2DHandle();
};

class ArrayTexture2DHandle : public ResourceHandle
{
public:
	ArrayTexture2DHandle();
	ArrayTexture2DHandle(ArrayTexture2DHandle&&) = default;
	~ArrayTexture2DHandle();
};

class MeshHandle : public ResourceHandle
{
public:
	MeshHandle();
	MeshHandle(MeshHandle&&) = default;
	~MeshHandle();
};

class RenderbufferHandle : public ResourceHandle
{
public:
	RenderbufferHandle();
	RenderbufferHandle(RenderbufferHandle&&) = default;
	~RenderbufferHandle();
};

class FramebufferHandle : public ResourceHandle
{
public:
	FramebufferHandle();
	FramebufferHandle(FramebufferHandle&&) = default;
	~FramebufferHandle();
};

class ShaderException : public std::runtime_error
{
public:
	ShaderException();
};

class VertexShader
{
public:
	explicit VertexShader(std::string_view contents);
	VertexShader(VertexShader&&) = default;

	auto id() const noexcept -> std::uint32_t;

private:
	VertexShaderHandle handle;
};

class GeometryShader
{
public:
	explicit GeometryShader(std::string_view contents);
	GeometryShader(GeometryShader&&) = default;

	auto id() const noexcept -> std::uint32_t;

private:
	GeometryShaderHandle handle;
};

class TessControlShader
{
public:
	explicit TessControlShader(std::string_view contents);
	TessControlShader(TessControlShader&&) = default;

	auto id() const noexcept -> std::uint32_t;

private:
	TessControlShaderHandle handle;
};

class TessEvaluationShader
{
public:
	explicit TessEvaluationShader(std::string_view contents);
	TessEvaluationShader(TessEvaluationShader&&) = default;

	auto id() const noexcept -> std::uint32_t;

private:
	TessEvaluationShaderHandle handle;
};

class FragmentShader
{
public:
	explicit FragmentShader(std::string_view contents);
	FragmentShader(FragmentShader&&) = default;

	auto id() const noexcept -> std::uint32_t;

private:
	FragmentShaderHandle handle;
};

class ComputeShader
{
public:
	explicit ComputeShader(std::string_view contents);
	ComputeShader(ComputeShader&&) = default;

	auto id() const noexcept -> std::uint32_t;

private:
	ComputeShaderHandle handle;
};

using Shader = std::variant<VertexShader,
		GeometryShader,
		TessControlShader,
		TessEvaluationShader,
		FragmentShader,
		ComputeShader>;

struct ShaderVec
{
	ShaderVec(std::vector<Shader>&& shaders)
	: impl(std::move(shaders))
	{
	}

	std::vector<Shader> impl;
};

/*
 * A fully compiled and linked shader program
 * Must be bound before issuing draw calls
 */
class ShaderProgram
{
public:
	ShaderProgram() = delete;
	explicit ShaderProgram(const ShaderVec& shaders);
	ShaderProgram(const VertexShader& vert, const FragmentShader& frag);
	ShaderProgram(ShaderProgram&& other) = default;
	~ShaderProgram() = default;

	void set_uniform(std::uint32_t location, int value);
	void set_uniform(std::uint32_t location, unsigned value);
	void set_uniform(std::uint32_t location, float value);
	void set_uniform(std::uint32_t location, double value);
	void set_uniform(std::uint32_t location, glm::vec2 value);
	void set_uniform(std::uint32_t location, glm::vec3 value);
	void set_uniform(std::uint32_t location, glm::vec4 value);
	void set_uniform(std::uint32_t location, const glm::mat4& value);

	void bind() const;

private:
	ShaderProgramHandle handle;
};

class FenceSync
{
public:
	FenceSync();
	FenceSync(FenceSync&& other) noexcept;
	~FenceSync();

	// deleted copy + assign
	FenceSync(const FenceSync& other) = delete;
	FenceSync& operator=(const FenceSync& other) = delete;
	FenceSync& operator=(FenceSync&& other) = delete;

	void resubmit();
	bool is_ready() const;

private:
	GLsync handle;
};

/*
 * GPU allocated static buffer
 */
class BufferBase
{
public:
	explicit BufferBase(gsl::span<const std::byte> buffer);

	void bind_to_uniform(std::size_t index) const;
	void bind_to_storage(std::size_t index) const;

	auto id() const noexcept -> std::uint32_t;
	auto size_bytes() const noexcept -> std::size_t;

private:
	std::size_t _size_bytes;
	BufferHandle handle;
};

/*
 * GPU allocated buffer
 */
template <class T>
class Buffer : public BufferBase
{
public:
	using value_type = T;

	explicit Buffer(const T& value)
	: BufferBase(gsl::span<const std::byte>(reinterpret_cast<std::byte*>(&value), sizeof(T)))
	{
	}
};

/*
 * GPU allocated array buffer
 */
template <class T>
class ArrayBuffer : public BufferBase
{
public:
	using value_type = T;

	explicit ArrayBuffer(const std::vector<T>& vec)
	: BufferBase(gsl::as_bytes(gsl::span<const T>(vec)))
	{
	}

	auto size() const noexcept -> std::size_t
	{
		return size_bytes() / sizeof(T);
	}
};

class BufferStreamBase
{
public:
	explicit BufferStreamBase(std::size_t size_bytes);

	void update(std::function<void(gsl::span<std::byte>)> f);

	void bind_to_uniform(std::size_t index) const;
	void bind_to_storage(std::size_t index) const;

	auto id() const noexcept -> std::uint32_t;
	auto current_slot() const noexcept -> std::size_t;
	auto slots() const noexcept -> std::size_t;
	auto size_bytes() const noexcept -> std::size_t;

protected:
	void write_lock_acquire();
	void write_lock_release();
	auto slot_span() const noexcept -> gsl::span<std::byte>;

private:
	std::array<FenceSync, 3> fences = {};
	gsl::span<std::byte> buffer;
	std::size_t slot;
	BufferHandle handle;
};

template <class T>
class BufferStream : public BufferStreamBase
{
public:
	using value_type = T;

	BufferStream()
	: BufferStreamBase(sizeof(T))
	{
	}

	template <class ... Args>
	void update(Args&& ... args)
	{
		write_lock_acquire();
		auto span = slot_span();
		auto casted_span = gsl::span<T>(reinterpret_cast<T*>(span.data()), 1);
		casted_span.front() = T{std::forward<Args>(args)...};
		write_lock_release();
	}
};

template <class T>
class ArrayBufferStream : public BufferStreamBase
{
public:
	using value_type = T;

	explicit ArrayBufferStream(std::size_t size)
	: BufferStreamBase(size * sizeof(T))
	{
	}

	void update(std::function<void(gsl::span<T>)> f)
	{
		write_lock_acquire();
		auto span = slot_span();
		auto casted_span = gsl::span<T>(reinterpret_cast<T*>(span.data()), span.size() / sizeof(T));
		f(casted_span);
		write_lock_release();
	}

	auto size() const noexcept -> std::size_t
	{
		return size_bytes() / sizeof(T);
	}
};

using IndexBuffer = ArrayBuffer<std::uint32_t>;

/*
 * A gpu side 2D texture
 * Must be bound before issuing draw calls
 */
class Texture2D
{
public:
	Texture2D(std::size_t width, std::size_t height, InternalFormat internal_format);
	explicit Texture2D(const Image& image, InternalFormat internal_format);

	void set_anti_aliasing(bool value);
	void bind(std::uint32_t unit = 0) const;

	auto id() const noexcept -> std::uint32_t;
	auto width() const noexcept -> std::size_t;
	auto height() const noexcept -> std::size_t;

private:
	Texture2DHandle handle;
	std::size_t _width;
	std::size_t _height;
};

class ArrayTexture2D
{
public:
	ArrayTexture2D(std::size_t width, std::size_t height, std::size_t layers, InternalFormat internal_format);
	ArrayTexture2D(const std::vector<Image>& images, InternalFormat internal_format);
	ArrayTexture2D(const std::vector<ImagePtr>& images, InternalFormat internal_format);
	ArrayTexture2D(const Image& sprite_sheet, std::size_t cell_width, InternalFormat internal_format);

	void set_anti_aliasing(bool value);
	void bind_to_unit(std::uint32_t unit) const;

	auto id() const noexcept -> std::uint32_t;
	auto width() const noexcept -> std::size_t;
	auto height() const noexcept -> std::size_t;
	auto layers() const noexcept -> std::size_t;

private:
	ArrayTexture2DHandle handle;
	std::size_t _width;
	std::size_t _height;
	std::size_t _layers;
};

/*
 * A gpu side object to contain vertex and index information
 * Must first be bound before issuing draw calls
 */
class MeshBase
{
public:
	void attach(const IndexBuffer& indices);

	auto id() const noexcept -> std::uint32_t;
	void bind() const;
	void unbind() const;

protected:
	void set_attrib_port(std::uint32_t attrib, std::uint32_t port);

	template <class Vec>
	void set_attrib_format(std::uint32_t attrib, std::size_t type_offset)
	{
		using T = typename Vec::value_type;
		set_attrib_format(attrib,
				type_to_int<T>(),
				detail::vec_dimensions<Vec>(),
				type_offset);
	}

	template <class T>
	void set_port_buffer(std::uint32_t port,
			const T& buffer,
			std::uint32_t offset)
	{
		set_port_buffer(port, buffer.id(), sizeof(typename T::value_type), offset);
	}

private:
	MeshHandle handle;

	template <class T>
	constexpr std::uint32_t type_to_int()
	{
		if constexpr (std::is_same_v<T, std::int8_t>)
			return 0x1400;
		if constexpr (std::is_same_v<T, std::uint8_t>)
			return 0x1401;
		if constexpr (std::is_same_v<T, std::int16_t>)
			return 0x1402;
		if constexpr (std::is_same_v<T, std::uint16_t>)
			return 0x1403;
		if constexpr (std::is_same_v<T, std::int32_t>)
			return 0x1404;
		if constexpr (std::is_same_v<T, std::uint32_t>)
			return 0x1405;
		if constexpr (std::is_same_v<T, float>)
			return 0x1406;
		throw std::logic_error("type to int");
	}

	void set_attrib_format(std::uint32_t attrib,
			std::uint32_t type,
			std::size_t type_count,
			std::size_t type_offset);

	void set_port_buffer(std::uint32_t port,
			std::uint32_t buffer,
			std::uint32_t type_size,
			std::uint32_t buffer_offset);
};

/*
 * Holds interleaved vertex data for draw calls
 *
 * Example: Buffer<tuple<vec3, vec3, vec2>> is in the format:
 * 	        { {vec3, vec3, vec2}, {vec3, vec3, vec2}, ... }
 * 	        Each tuple represents a single vertex to send to the shader
 */
template <class ... Ts>
class Mesh : public MeshBase
{
public:
	using value_type = std::tuple<Ts...>;

	Mesh(const std::vector<value_type>& vertices, const IndexBuffer& indices)
	: vertices(vertices)
	{
		attach_vertices(std::make_index_sequence<sizeof...(Ts)>());
		MeshBase::attach(indices);
	}

	Mesh(ArrayBuffer<value_type>&& vertices, const IndexBuffer& indices)
	: vertices(std::move(vertices))
	{
		attach_vertices(std::make_index_sequence<sizeof...(Ts)>());
		MeshBase::attach(indices);
	}

private:
	ArrayBuffer<value_type> vertices;

	template <std::size_t ... Is>
	void attach_vertices(std::index_sequence<Is...>)
	{
		set_port_buffer(0, vertices, 0);
		TUPLE_FOR_EACH(set_attrib_format<TUPLE_TYPE(Is, value_type)>(Is, TUPLE_OFFSET(Is, value_type)));
		TUPLE_FOR_EACH(set_attrib_port(Is, 0));
	}
};

template <class ... Ts>
class MeshStream : public MeshBase
{
public:
	using value_type = std::tuple<Ts...>;

	MeshStream(std::size_t vertices, const IndexBuffer& indices)
	: vertices(vertices)
	{
		attach_vertices(std::make_index_sequence<sizeof...(Ts)>());
		MeshBase::attach(indices);
	}

	MeshStream(ArrayBufferStream<value_type>&& vertices, const IndexBuffer& indices)
	: vertices(std::move(vertices))
	{
		attach_vertices(std::make_index_sequence<sizeof...(Ts)>());
		MeshBase::attach(indices);
	}

	void update(std::function<void(gsl::span<value_type>)> f)
	{
		vertices.update(f);
		update_ports(vertices.current_slot(), std::make_index_sequence<sizeof...(Ts)>());
	}

private:
	ArrayBufferStream<value_type> vertices;

	template <std::size_t ... Is>
	void attach_vertices(std::index_sequence<Is...>)
	{
		// Bind each buffer slot to a different port
		for (std::size_t i = 0; i < vertices.slots(); ++i)
			set_port_buffer(i, vertices, i * vertices.size_bytes());

		// Init attrib formats/ports
		TUPLE_FOR_EACH(set_attrib_format<TUPLE_TYPE(Is, value_type)>(Is, TUPLE_OFFSET(Is, value_type)));
		TUPLE_FOR_EACH(set_attrib_port(Is, vertices.current_slot()));
	}

	template <std::size_t ... Is>
	void update_ports(std::uint32_t port, std::index_sequence<Is...>)
	{
		TUPLE_FOR_EACH(set_attrib_port(Is, port));
	}
};

/*
 * A logical Framebuffer attachment
 */
class Renderbuffer
{
public:
	Renderbuffer(std::size_t width, std::size_t height, InternalFormat internal_format);

	auto width() const noexcept -> std::size_t;
	auto height() const noexcept -> std::size_t;
	auto id() const noexcept -> std::uint32_t;

private:
	RenderbufferHandle handle;
	std::size_t _width;
	std::size_t _height;
};

/*
 * A canvas to issue draw calls to when the default framebuffer is inadequate
 */
class Framebuffer
{
public:
	Framebuffer() = default;

	void attach(const Renderbuffer& buffer, FramebufferAttachment attachment);
	void attach(const Texture2D& texture, FramebufferAttachment attachment);
	void clear(glm::vec4 rgba, float depth = 1.0f, float stencil = 0.0f);

	void blit(Framebuffer& destination) const;
	void blit(int width, int height) const;

	void bind() const;

	auto width() const noexcept -> std::size_t;
	auto height() const noexcept -> std::size_t;

private:
	FramebufferHandle handle;
	std::size_t _width = 0;
	std::size_t _height = 0;
};

// Clear the default framebuffer
void clear(glm::vec4 rgba, float depth = 1.0f);

// Draw n indices in triangles from the currently bound Mesh
void draw_triangles(std::size_t n);
void draw_triangle_strips(std::size_t n);
void draw_triangle_strips_instanced(std::size_t indices, std::size_t instances);

// Enable alpha blending
void set_alpha(bool value);

// Get a version string
auto get_version() -> std::string;

// Get a renderer string
auto get_renderer() -> std::string;

} // namespace ori

#undef TUPLE_TYPE
#undef TUPLE_OFFSET
#undef TUPLE_FOR_EACH

#endif // WRAPPERS_HPP_
