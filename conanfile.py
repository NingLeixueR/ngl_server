# File overview: Declares Conan dependencies and generators for ngl_server.

from conan import ConanFile

class MyProjectConan(ConanFile):
    # 1. Data (, )
    name = "my_project"
    version = "1.0"
    
    # 2. Settings, Conan current /
    settings = "os", "compiler", "build_type", "arch"
    
    # 3., Corresponding txt in [generators]
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # --- (All need to) ---
        self.requires("boost/1.86.0")
        self.requires("lua/5.4.6")
        self.requires("protobuf/3.21.12")
        self.requires("libcurl/8.9.1")
        self.requires("libmysqlclient/8.1.0")
        self.requires("hiredis/1.2.0")
        self.requires("libpq/17.7")
        self.requires("rapidjson/1.1.0")
        
        # Translated comment.
        if self.settings.os == "Linux":
            self.requires("libuuid/1.0.3")

    def build_requirements(self):
        self.test_requires("gtest/1.14.0")