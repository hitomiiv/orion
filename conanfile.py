from conans import ConanFile, CMake, tools

class OrionConan(ConanFile):
    name = "orion"
    version = "0.1"
    license = "Apache 2.0 License"
    author = "Sabrina Gregory"
    url = "https://github.com/hitomiiv/orion.git"
    description = "Modern OpenGL rendering library"
    topics = ("opengl")
    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "src/*"

    default_options = {"shared": False,
       "fPIC": True,
       "glad:gl_profile": "compatibility",
       "glad:gl_version": 4.6,
       "glad:no_loader": False,
       "spdlog:shared": True,
       "fmt:shared": True}
    
    requires = ["glad/0.1.34",
        "glfw/3.3.4",
        "glm/0.9.9.8",
        "ms-gsl/3.1.0",
        "fmt/8.0.1",
        "spdlog/1.9.2",
        "stb/cci.20210713"]
    
    options = {"shared": [True, False], "fPIC": [True, False]}
    
    generators = "cmake_find_package"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="src")
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="src")
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["orion"]
        
