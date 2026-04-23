from conan import ConanFile

class NglServerConan(ConanFile):
    name = "ngl_server"
    version = "0.1"
    package_type = "application"
    license = "MIT"
    url = "https://github.com/NingLeixueR/ngl_server"
    description = "Actor-model C++ game server framework"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    default_options = {
        "behaviortree.cpp/*:enable_groot_interface": False,
        "behaviortree.cpp/*:enable_sqlite_logging": False,
        "behaviortree.cpp/*:with_tools": False,
    }

    def requirements(self):
        self.requires("boost/1.86.0")
        self.requires("behaviortree.cpp/4.9.0")
        self.requires("tinyxml2/11.0.0")
        self.requires("lua/5.4.6")
        self.requires("protobuf/3.21.12")
        self.requires("libcurl/8.9.1")
        self.requires("libmysqlclient/8.1.0")
        self.requires("hiredis/1.2.0")
        self.requires("libpq/17.7")
        self.requires("rapidjson/1.1.0")

        if self.settings.os == "Linux":
            self.requires("libuuid/1.0.3")

    def build_requirements(self):
        self.test_requires("gtest/1.14.0")
