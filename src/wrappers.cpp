// utils.cpp
#include "wrappers.hpp"

#include <stdexcept>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <glad/glad.h>

#include "Logger.hpp"

#if __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#	pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if __GNUC__
#	pragma GCC diagnostic pop
#endif

namespace ori
{

template <class E>
static auto to_underlying(E value) noexcept -> std::uint32_t
{
	return static_cast<std::uint32_t>(value);
}

void ImageDeleter::operator()(Image* image) const noexcept
{
	if (image->data)
		stbi_image_free(image->data);
	delete image;
}

static int num_components(ImageFormat f)
{
	switch (f)
	{
	case ImageFormat::r:
		return 1;
	case ImageFormat::rg:
		return 2;
	case ImageFormat::rgb:
		return 3;
	case ImageFormat::rgba:
		return 4;
	case ImageFormat::bgr:
		return 3;
	case ImageFormat::bgra:
		return 4;
	default: assert(false); return 0;
	}
}

ImageException::ImageException()
: std::runtime_error("Image exception")
{
}

auto Image::load(std::string_view path, ImageFormat format, bool flip_vertically)
-> ImagePtr
{
	stbi_set_flip_vertically_on_load(flip_vertically);

	auto image = ImagePtr(new Image);

	if (format == ImageFormat::deduce)
	{
		auto ext = path.substr(path.size() - 3);
		if (ext == "png")
			format = ImageFormat::rgba;
		else if (ext == "jpg")
			format = ImageFormat::rgb;
		else if (ext == "bmp")
			format = ImageFormat::bgr;
		else
		{
			Logger::get().error({"Unable to deduce image format from {}"}, path);
			throw ImageException();
		}
	}

	int components;
	image->data = stbi_load(path.data(),
			&image->width,
			&image->height,
			&components,
			num_components(format));
	image->format = format;

	if (image->data == nullptr)
	{
		Logger::get().error({"Unable to open {}: {}"}, path, stbi_failure_reason());
		throw ImageException();
	}

	return image;
}

ResourceHandle::ResourceHandle(std::uint32_t __id) noexcept
: _id(__id)
{
}

ResourceHandle::ResourceHandle(ResourceHandle&& other) noexcept
{
	_id = other._id;
	other._id = 0;
}

auto ResourceHandle::id() const noexcept -> std::uint32_t
{
	return _id;
}

bool ResourceHandle::operator ==(const ResourceHandle& other) const noexcept
{
	return _id == other._id;
}

bool ResourceHandle::operator !=(const ResourceHandle& other) const noexcept
{
	return _id != other._id;
}

VertexShaderHandle::VertexShaderHandle()
{
	_id = glCreateShader(GL_VERTEX_SHADER);
}

VertexShaderHandle::~VertexShaderHandle()
{
	if (_id)
	{
		glDeleteShader(_id);
		_id = 0;
	}
}

FragmentShaderHandle::FragmentShaderHandle()
{
	_id = glCreateShader(GL_FRAGMENT_SHADER);
}

FragmentShaderHandle::~FragmentShaderHandle()
{
	if (_id)
	{
		glDeleteShader(_id);
		_id = 0;
	}
}

GeometryShaderHandle::GeometryShaderHandle()
{
	_id = glCreateShader(GL_GEOMETRY_SHADER);
}

GeometryShaderHandle::~GeometryShaderHandle()
{
	if (_id)
	{
		glDeleteShader(_id);
		_id = 0;
	}
}

TessControlShaderHandle::TessControlShaderHandle()
{
	_id = glCreateShader(GL_TESS_CONTROL_SHADER);
}

TessControlShaderHandle::~TessControlShaderHandle()
{
	if (_id)
	{
		glDeleteShader(_id);
		_id = 0;
	}
}

TessEvaluationShaderHandle::TessEvaluationShaderHandle()
{
	_id = glCreateShader(GL_TESS_EVALUATION_SHADER);
}

TessEvaluationShaderHandle::~TessEvaluationShaderHandle()
{
	if (_id)
	{
		glDeleteShader(_id);
		_id = 0;
	}
}

ComputeShaderHandle::ComputeShaderHandle()
{
	_id = glCreateShader(GL_COMPUTE_SHADER);
}

ComputeShaderHandle::~ComputeShaderHandle()
{
	if (_id)
	{
		glDeleteShader(_id);
		_id = 0;
	}
}

ShaderProgramHandle::ShaderProgramHandle()
{
	_id = glCreateProgram();
}

ShaderProgramHandle::~ShaderProgramHandle()
{
	if (_id)
	{
		glDeleteProgram(_id);
		_id = 0;
	}
}

BufferHandle::BufferHandle()
{
	glCreateBuffers(1, &_id);
}

BufferHandle::~BufferHandle()
{
	if (_id)
	{
		glDeleteBuffers(1, &_id);
		_id = 0;
	}
}

Texture2DHandle::Texture2DHandle()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &_id);
}

Texture2DHandle::~Texture2DHandle()
{
	if (_id)
	{
		glDeleteTextures(1, &_id);
		_id = 0;
	}
}

ArrayTexture2DHandle::ArrayTexture2DHandle()
{
	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_id);
}

ArrayTexture2DHandle::~ArrayTexture2DHandle()
{
	if (_id)
	{
		glDeleteTextures(1, &_id);
		_id = 0;
	}
}

MeshHandle::MeshHandle()
{
	glCreateVertexArrays(1, &_id);
}

MeshHandle::~MeshHandle()
{
	if (_id)
	{
		glDeleteVertexArrays(1, &_id);
		_id = 0;
	}
}

FramebufferHandle::FramebufferHandle()
{
	glCreateFramebuffers(1, &_id);
}

FramebufferHandle::~FramebufferHandle()
{
	if (_id)
	{
		glDeleteFramebuffers(1, &_id);
		_id = 0;
	}
}

RenderbufferHandle::RenderbufferHandle()
{
	glCreateRenderbuffers(1, &_id);
}

RenderbufferHandle::~RenderbufferHandle()
{
	if (_id)
	{
		glDeleteRenderbuffers(1, &_id);
		_id = 0;
	}
}

static void compile_shader(std::uint32_t id, std::string_view contents)
{
	const char* contents_cstr = contents.data();
	glShaderSource(id, 1, &contents_cstr, nullptr);
	glCompileShader(id);

	GLint is_compiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &is_compiled);
	if (!is_compiled)
	{
		GLint length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		auto message = std::make_unique<char[]>(length);
		glGetShaderInfoLog(id, length, &length, message.get());

		auto view = std::string_view(message.get(), length);
		Logger::get().error({"Shader compilation failed:\n{}"}, view);
		throw ShaderException();
	}
}

ShaderException::ShaderException()
: std::runtime_error("Shader exception")
{
}

VertexShader::VertexShader(std::string_view contents)
{
	compile_shader(handle.id(), contents);
}

auto VertexShader::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

GeometryShader::GeometryShader(std::string_view contents)
{
	compile_shader(handle.id(), contents);
}

auto GeometryShader::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

TessControlShader::TessControlShader(std::string_view contents)
{
	compile_shader(handle.id(), contents);
}

auto TessControlShader::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

TessEvaluationShader::TessEvaluationShader(std::string_view contents)
{
	compile_shader(handle.id(), contents);
}

auto TessEvaluationShader::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

FragmentShader::FragmentShader(std::string_view contents)
{
	compile_shader(handle.id(), contents);
}

auto FragmentShader::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

ComputeShader::ComputeShader(std::string_view contents)
{
	compile_shader(handle.id(), contents);
}

auto ComputeShader::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

void ensure_linkage(std::uint32_t program)
{
	GLint is_linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
	if (!is_linked)
	{
		GLint length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		auto message = std::make_unique<char[]>(length);
		glGetProgramInfoLog(program, length, &length, message.get());

		auto view = std::string_view(message.get(), length);
		Logger::get().error({"Shader linkage failed:\n{}"}, view);
		throw ShaderException();
	}
}

ShaderProgram::ShaderProgram(const ShaderVec& shaders)
{
	auto attach_shader = [this](const auto& shader)
	{
		glAttachShader(handle.id(), shader.id());
	};

	auto detach_shader = [this](const auto& shader)
	{
		glDetachShader(handle.id(), shader.id());
	};

	for (const auto& shader : shaders.impl)
		std::visit(attach_shader, shader);

	glLinkProgram(handle.id());

	for (const auto& shader : shaders.impl)
		std::visit(detach_shader, shader);

	ensure_linkage(handle.id());
}

ShaderProgram::ShaderProgram(const VertexShader& vert, const FragmentShader& frag)
{
	glAttachShader(handle.id(), vert.id());
	glAttachShader(handle.id(), frag.id());
	glLinkProgram(handle.id());
	glDetachShader(handle.id(), vert.id());
	glDetachShader(handle.id(), frag.id());

	ensure_linkage(handle.id());
}

void ShaderProgram::set_uniform(std::uint32_t location, int value)
{
	glProgramUniform1i(handle.id(), location, value);
}

void ShaderProgram::set_uniform(std::uint32_t location, unsigned value)
{
	glProgramUniform1ui(handle.id(), location, value);
}

void ShaderProgram::set_uniform(std::uint32_t location, float value)
{
	glProgramUniform1f(handle.id(), location, value);
}

void ShaderProgram::set_uniform(std::uint32_t location, double value)
{
	glProgramUniform1d(handle.id(), location, value);
}

void ShaderProgram::set_uniform(std::uint32_t location, glm::vec2 value)
{
	glProgramUniform2f(handle.id(), location, value.x, value.y);
}

void ShaderProgram::set_uniform(std::uint32_t location, glm::vec3 value)
{
	glProgramUniform3f(handle.id(), location, value.x, value.y, value.z);
}

void ShaderProgram::set_uniform(std::uint32_t location, glm::vec4 value)
{
	glProgramUniform4f(handle.id(), location, value.x, value.y, value.z, value.w);
}

void ShaderProgram::set_uniform(std::uint32_t location, const glm::mat4& value)
{
	glProgramUniformMatrix4fv(handle.id(), location, 1, false, &value[0][0]);
}

void ShaderProgram::bind() const
{
	glUseProgram(handle.id());
}

FenceSync::FenceSync()
{
	handle = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

FenceSync::FenceSync(FenceSync&& other) noexcept
{
	handle = other.handle;
	other.handle = nullptr;
}

FenceSync::~FenceSync()
{
	if (handle)
		glDeleteSync(handle);
}

void FenceSync::resubmit()
{
	glDeleteSync(handle);
	handle = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

bool FenceSync::is_ready() const
{
	auto res = glClientWaitSync(handle, 0, 0);
	return res == GL_CONDITION_SATISFIED || res == GL_ALREADY_SIGNALED;
}

BufferBase::BufferBase(gsl::span<const std::byte> buffer)
: _size_bytes(buffer.size_bytes())
{
	glNamedBufferStorage(handle.id(), _size_bytes, buffer.data(), 0);
}

void BufferBase::bind_to_uniform(std::size_t index) const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, handle.id());
}

void BufferBase::bind_to_storage(std::size_t index) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, handle.id());
}

auto BufferBase::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

auto BufferBase::size_bytes() const noexcept -> std::size_t
{
	return _size_bytes;
}

BufferStreamBase::BufferStreamBase(std::size_t size_bytes)
{
	const GLenum flags = GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

	size_bytes *= slots();
	glNamedBufferStorage(handle.id(), size_bytes, nullptr, flags);
	auto data = (std::byte*) glMapNamedBufferRange(handle.id(), 0, size_bytes, flags);
	buffer = { data, size_bytes };
	slot = 0;
}

void BufferStreamBase::update(std::function<void(gsl::span<std::byte>)> f)
{
	write_lock_acquire();
	f(slot_span());
	write_lock_release();
}

void BufferStreamBase::bind_to_uniform(std::size_t index) const
{
	glBindBufferRange(GL_UNIFORM_BUFFER,
	 	index,
	 	handle.id(),
	 	slot * size_bytes(),
		size_bytes());
}

void BufferStreamBase::bind_to_storage(std::size_t index) const
{
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
	 	index,
	 	handle.id(),
	 	slot * size_bytes(),
		size_bytes());
}

auto BufferStreamBase::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

auto BufferStreamBase::current_slot() const noexcept -> std::size_t
{
	return slot;
}

auto BufferStreamBase::slots() const noexcept -> std::size_t
{
	return fences.size();
}

auto BufferStreamBase::size_bytes() const noexcept -> std::size_t
{
	return buffer.size() / slots();
}

void BufferStreamBase::write_lock_acquire()
{
	slot = (slot + 1) % slots();
	while (!fences[slot].is_ready());
}

void BufferStreamBase::write_lock_release()
{
	glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
	fences[slot].resubmit();
}

auto BufferStreamBase::slot_span() const noexcept -> gsl::span<std::byte>
{
	return buffer.subspan(slot * size_bytes(), size_bytes());
}

Texture2D::Texture2D(std::size_t __width, std::size_t __height, InternalFormat internal_format)
: _width(__width)
, _height(__height)
{
	set_anti_aliasing(false);
	std::size_t levels = std::log2(std::max(__width, __height)) + 1;
	glTextureStorage2D(handle.id(), levels, to_underlying(internal_format), _width, _height);
	glGenerateTextureMipmap(handle.id());
}

Texture2D::Texture2D(const Image& image, InternalFormat internal_format)
: Texture2D(image.width, image.height, internal_format)
{
	glTextureSubImage2D(handle.id(),
		 	0,
		 	0,
		 	0,
		 	_width,
		 	_height,
		 	to_underlying(image.format),
		 	GL_UNSIGNED_BYTE,
		 	image.data);
}

void Texture2D::set_anti_aliasing(bool value)
{
	if (value)
	{
		glTextureParameteri(handle.id(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(handle.id(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTextureParameteri(handle.id(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(handle.id(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

auto Texture2D::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

auto Texture2D::width() const noexcept -> std::size_t
{
	return _width;
}

auto Texture2D::height() const noexcept -> std::size_t
{
	return _height;
}

void Texture2D::bind(std::uint32_t unit) const
{
	glBindTextureUnit(unit, handle.id());
}

ArrayTexture2D::ArrayTexture2D(std::size_t __width,
		std::size_t __height, std::size_t __depth, InternalFormat internal_format)
: _width(__width), _height(__height), _layers(__depth)
{
	set_anti_aliasing(false);
	glTextureStorage3D(handle.id(),
	 	1,
	 	to_underlying(internal_format),
	 	_width,
	 	_height,
	 	_layers);
}

ArrayTexture2D::ArrayTexture2D(const std::vector<Image>& images, InternalFormat internal_format)
: ArrayTexture2D(images.front().width,
				 images.front().height,
				 images.size(),
				 internal_format)
{
	for (std::size_t i = 0; i < images.size(); ++i)
	{
		if (!(images[i].width == (int) _width) ||
			!(images[i].height == (int) _height))
		{
			Logger::get().error({"Image {:d} does not match specified dimensions"}, i);
			throw ImageException();
		}

		glTextureSubImage3D(handle.id(),
		 	0,
		 	0,
		 	0,
		 	i,
		 	_width,
		 	_height,
		 	1,
		 	to_underlying(images[i].format),
		 	GL_UNSIGNED_BYTE,
		 	images[i].data);
	}
}

ArrayTexture2D::ArrayTexture2D(const std::vector<ImagePtr>& images, InternalFormat internal_format)
: ArrayTexture2D(images.front()->width,
				 images.front()->height,
				 images.size(),
				 internal_format)
{
	for (std::size_t i = 0; i < images.size(); ++i)
	{
		if (images[i]->width > (int) _width
			|| images[i]->height > (int) _height)
		{
			Logger::get().error({"Image {:d} is greater than specified dimensions"}, i);
			throw ImageException();
		}

		glTextureSubImage3D(handle.id(),
			0,
			0,
			0,
			i,
			_width,
			_height,
			1,
			to_underlying(images[i]->format),
			GL_UNSIGNED_BYTE,
			images[i]->data);
	}
}

ArrayTexture2D::ArrayTexture2D(const Image& sprite_sheet, std::size_t cell_width, InternalFormat internal_format)
: ArrayTexture2D(cell_width,
		cell_width,
		std::pow(sprite_sheet.width / cell_width, 2),
		internal_format)
{
	auto flat = Texture2D(sprite_sheet, internal_format);

	const std::size_t rows = sprite_sheet.width / cell_width;

	for (std::size_t i = 0; i < _layers; ++i)
	{
		std::size_t x = (i % rows) * cell_width;
		std::size_t y = (rows - 1 - i / rows) * cell_width;

		glCopyImageSubData(
			flat.id(), GL_TEXTURE_2D, 0,
		 	x, y, 0,
		 	handle.id(), GL_TEXTURE_2D_ARRAY, 0,
		 	0, 0, i,
			cell_width, cell_width, 1);
	}
}

void ArrayTexture2D::set_anti_aliasing(bool value)
{
	if (value)
	{
		glTextureParameteri(handle.id(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(handle.id(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTextureParameteri(handle.id(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(handle.id(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

auto ArrayTexture2D::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

auto ArrayTexture2D::width() const noexcept -> std::size_t
{
	return _width;
}

auto ArrayTexture2D::height() const noexcept -> std::size_t
{
	return _height;
}

auto ArrayTexture2D::layers() const noexcept -> std::size_t
{
	return _layers;
}

void ArrayTexture2D::bind_to_unit(std::uint32_t unit) const
{
	glBindTextureUnit(unit, handle.id());
}

void MeshBase::attach(const IndexBuffer& indices)
{
	glVertexArrayElementBuffer(handle.id(), indices.id());
}

void MeshBase::bind() const
{
	glBindVertexArray(handle.id());
}

void MeshBase::unbind() const
{
	glBindVertexArray(0);
}

auto MeshBase::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

void MeshBase::set_attrib_port(std::uint32_t attrib, std::uint32_t port)
{
	glVertexArrayAttribBinding(id(), attrib, port);
}

void MeshBase::set_attrib_format(std::uint32_t attrib,
		std::uint32_t type,
		std::size_t type_count,
		std::size_t type_offset)
{
	glEnableVertexArrayAttrib(id(), attrib);
	glVertexArrayAttribFormat(id(), attrib, type_count, type, false, type_offset);
}

void MeshBase::set_port_buffer(std::uint32_t port,
		std::uint32_t buffer,
		std::uint32_t type_size,
		std::uint32_t buffer_offset)
{
	glVertexArrayVertexBuffer(id(), port, buffer, buffer_offset, type_size);
}

Renderbuffer::Renderbuffer(std::size_t __width, std::size_t __height, InternalFormat internal_format)
: _width(__width)
, _height(__height)
{
	glNamedRenderbufferStorage(handle.id(), to_underlying(internal_format), __width, __height);
}

auto Renderbuffer::width() const noexcept -> std::size_t
{
	return _width;
}

auto Renderbuffer::height() const noexcept -> std::size_t
{
	return _height;
}

auto Renderbuffer::id() const noexcept -> std::uint32_t
{
	return handle.id();
}

void Framebuffer::attach(const Renderbuffer& buffer, FramebufferAttachment attachment)
{
	_width = buffer.width();
	_height = buffer.height();
	glNamedFramebufferRenderbuffer(handle.id(),
			to_underlying(attachment),
			GL_RENDERBUFFER,
			buffer.id());
}

void Framebuffer::attach(const Texture2D& texture, FramebufferAttachment attachment)
{
	_width = texture.width();
	_height = texture.height();
	glNamedFramebufferTexture(handle.id(),
			to_underlying(attachment),
			texture.id(),
		 	0);
}

void Framebuffer::clear(glm::vec4 rgba, float depth, float stencil)
{
	glClearNamedFramebufferfv(handle.id(), GL_COLOR, 0, &rgba[0]);
	glClearNamedFramebufferfv(handle.id(), GL_DEPTH, 0, &depth);
	glClearNamedFramebufferfv(handle.id(), GL_STENCIL, 0, &stencil);
	// FIXME separate functions
}

void Framebuffer::blit(Framebuffer& destination) const
{
	glBlitNamedFramebuffer(handle.id(),
	 	destination.handle.id(),
	 	0,
	 	0,
	 	_width,
	 	_height,
	 	0,
	 	0,
	 	destination._width,
	 	destination._height,
	 	GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
	 	GL_NEAREST);
}

void Framebuffer::blit(int __width, int __height) const
{
	glBlitNamedFramebuffer(handle.id(),
	 	0,
	 	0,
	 	0,
	 	_width,
	 	_height,
	 	0,
	 	0,
		__width,
		__height,
	 	GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
	 	GL_NEAREST);
}

auto Framebuffer::width() const noexcept -> std::size_t
{
	return _width;
}

auto Framebuffer::height() const noexcept -> std::size_t
{
	return _height;
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, handle.id());
}

void clear(glm::vec4 rgba, float depth)
{
	glClearNamedFramebufferfv(0, GL_COLOR, 0, &rgba[0]);
	glClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth);
}

void draw_triangles(std::size_t n)
{
	glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
}

void draw_triangle_strips(std::size_t n)
{
	glDrawElements(GL_TRIANGLE_STRIP, n, GL_UNSIGNED_INT, 0);
}

void draw_triangle_strips_instanced(std::size_t indices, std::size_t instances)
{
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, indices, GL_UNSIGNED_INT, 0, instances);
}

void set_alpha(bool value)
{
	if (value)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

auto get_version() -> std::string
{
	std::string str = "OpenGL ";
	str += std::string((const char*) glGetString(GL_VERSION));
	return str;
}

auto get_renderer() -> std::string
{
	return (const char*) glGetString(GL_RENDERER);
}

} // namespace ori
