from conan import ConanFile

class MyProjectConan(ConanFile):
    # 1. 定义项目元数据 (可选，但推荐)
    name = "my_project"
    version = "1.0"
    
    # 2. 必须声明 settings，否则 Conan 不知道当前是什么系统/编译器
    settings = "os", "compiler", "build_type", "arch"
    
    # 3. 声明生成器，对应原 txt 中的 [generators]
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # --- 通用依赖 (所有平台都需要) ---
        self.requires("boost/1.86.0")
        self.requires("lua/5.4.6")
        self.requires("protobuf/3.21.12")
        self.requires("libcurl/8.9.1")
        self.requires("libmysqlclient/8.1.0")
        self.requires("hiredis/1.2.0")
        self.requires("libpq/17.7")
        self.requires("rapidjson/1.1.0")
        
        # --- 平台特定依赖 ---
        if self.settings.os == "Linux":
            self.requires("libuuid/1.0.3")

    def build_requirements(self):
        self.test_requires("gtest/1.14.0")