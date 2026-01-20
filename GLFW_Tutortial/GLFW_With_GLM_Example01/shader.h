#ifndef SHADER_SHADER_H_
#define SHADER_SHADER_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

#include <GL/glew.h>

class Shader {
private:
	GLuint program_;
	bool is_spirv_;
	
	// 禁止复制构造和赋值
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	
public:
	// ========== 构造函数 ==========
	
	// 从GLSL字符串编译
	Shader(const std::string& vertex_code, const std::string& fragment_code, 
		   bool from_file = false) 
	: program_(0), is_spirv_(false) {
		if (from_file) {
			std::string v_code = ReadFile(vertex_code);
			std::string f_code = ReadFile(fragment_code);
			CompileGlsl(v_code, f_code);
		} else {
			CompileGlsl(vertex_code, fragment_code);
		}
	}
	
	// 从SPIR-V文件加载
	Shader(const std::string& vertex_spv_path, const std::string& fragment_spv_path, 
		   const char* /*tag*/) 
	: program_(0), is_spirv_(true) {
		LoadSpirV(vertex_spv_path, fragment_spv_path);
	}
	
	// ========== 移动语义 ==========
	
	// 移动构造函数
	Shader(Shader&& other) noexcept 
	: program_(other.program_), is_spirv_(other.is_spirv_) {
		other.program_ = 0;
		other.is_spirv_ = false;
	}
	
	// 移动赋值运算符
	Shader& operator=(Shader&& other) noexcept {
		if (this != &other) {
			if (program_ != 0) {
				glDeleteProgram(program_);
			}
			
			program_ = other.program_;
			is_spirv_ = other.is_spirv_;
			
			other.program_ = 0;
			other.is_spirv_ = false;
		}
		return *this;
	}
	
	// ========== 静态工厂方法 ==========
	
	// 从GLSL字符串创建
	static Shader CreateFromGlslString(const std::string& vertex_code, 
									   const std::string& fragment_code) {
		return Shader(vertex_code, fragment_code, false);
	}
	
	// 从GLSL文件创建
	static Shader CreateFromGlslFile(const std::string& vertex_path, 
									 const std::string& fragment_path) {
		return Shader(vertex_path, fragment_path, true);
	}
	
	// 从SPIR-V文件创建
	static Shader CreateFromSpirV(const std::string& vertex_spv_path, 
								  const std::string& fragment_spv_path) {
		return Shader(vertex_spv_path, fragment_spv_path, "spirv");
	}
	
	// ========== 析构函数 ==========
	~Shader() {
		if (program_ != 0) {
			glDeleteProgram(program_);
		}
	}
	
	// ========== 公有方法 ==========
	
	// 使用着色器
	void Use() const {
		if (program_ != 0) {
			glUseProgram(program_);
		}
	}
	
	// 停止使用着色器
	static void UseNone() {
		glUseProgram(0);
	}
	
	// 获取程序ID
	GLuint GetProgram() const { return program_; }
	
	// 检查是否有效
	bool IsValid() const { return program_ != 0; }
	
	// 获取Uniform位置
	GLint GetUniformLocation(const std::string& name) const {
		if (program_ == 0) return -1;
		return glGetUniformLocation(program_, name.c_str());
	}
	
	// 设置Uniform的便捷函数
	void SetUniform(const std::string& name, int value) const {
		GLint loc = GetUniformLocation(name);
		if (loc != -1) glUniform1i(loc, value);
	}
	
	void SetUniform(const std::string& name, float value) const {
		GLint loc = GetUniformLocation(name);
		if (loc != -1) glUniform1f(loc, value);
	}
	
	void SetUniform(const std::string& name, float x, float y) const {
		GLint loc = GetUniformLocation(name);
		if (loc != -1) glUniform2f(loc, x, y);
	}
	
	void SetUniform(const std::string& name, float x, float y, float z) const {
		GLint loc = GetUniformLocation(name);
		if (loc != -1) glUniform3f(loc, x, y, z);
	}
	
private:
	// ========== 私有辅助方法 ==========
	
	// 读取文件
	static std::string ReadFile(const std::string& file_path) {
		std::string content;
		std::ifstream file(file_path);
		
		if (!file.is_open()) {
			std::cerr << "ERROR::SHADER::FILE_NOT_OPENED: " << file_path << std::endl;
			return content;
		}
		
		std::stringstream stream;
		stream << file.rdbuf();
		content = stream.str();
		file.close();
		
		return content;
	}
	
	// 读取二进制文件（用于SPIR-V）
	static std::vector<char> ReadBinaryFile(const std::string& file_path) {
		std::ifstream file(file_path, std::ios::binary | std::ios::ate);
		std::vector<char> buffer;
		
		if (!file.is_open()) {
			std::cerr << "ERROR::SHADER::BINARY_FILE_NOT_OPENED: " 
			<< file_path << std::endl;
			return buffer;
		}
		
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		buffer.resize(size);
		
		if (!file.read(buffer.data(), size)) {
			std::cerr << "ERROR::SHADER::BINARY_FILE_READ_FAILED: " 
			<< file_path << std::endl;
			buffer.clear();
		}
		
		file.close();
		return buffer;
	}
	
	// 编译GLSL着色器
	void CompileGlsl(const std::string& vertex_shader_code, 
					 const std::string& fragment_shader_code) {
		// 编译顶点着色器
		GLuint vertex = CompileSingleShader(GL_VERTEX_SHADER, vertex_shader_code, 
											"vertex");
		if (vertex == 0) return;
		
		// 编译片段着色器
		GLuint fragment = CompileSingleShader(GL_FRAGMENT_SHADER, 
											  fragment_shader_code, "fragment");
		if (fragment == 0) {
			glDeleteShader(vertex);
			return;
		}
		
		// 创建程序并链接
		program_ = glCreateProgram();
		glAttachShader(program_, vertex);
		glAttachShader(program_, fragment);
		glLinkProgram(program_);
		
		// 检查链接状态
		GLint success;
		GLchar info_log[512];
		glGetProgramiv(program_, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program_, sizeof(info_log), nullptr, info_log);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" 
			<< info_log << std::endl;
			
			// 清理
			glDetachShader(program_, vertex);
			glDetachShader(program_, fragment);
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			glDeleteProgram(program_);
			program_ = 0;
			return;
		}
		
		// 成功链接后删除着色器对象
		glDetachShader(program_, vertex);
		glDetachShader(program_, fragment);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		
		std::cout << "GLSL shader compiled successfully (ID: " 
		<< program_ << ")" << std::endl;
	}
	
	// 编译单个着色器（辅助函数）
	static GLuint CompileSingleShader(GLenum type, const std::string& source, 
									  const std::string& shader_type) {
		GLuint shader = glCreateShader(type);
		
		const GLchar* source_ptr = source.c_str();
		glShaderSource(shader, 1, &source_ptr, nullptr);
		glCompileShader(shader);
		
		// 检查编译状态
		GLint success;
		GLchar info_log[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		
		if (!success) {
			glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
			std::cerr << "ERROR::SHADER::" << shader_type 
			<< "::COMPILATION_FAILED\n" << info_log << std::endl;
			glDeleteShader(shader);
			return 0;
		}
		
		return shader;
	}
	
	// 加载SPIR-V着色器
	void LoadSpirV(const std::string& vertex_spv_path, 
				   const std::string& fragment_spv_path) {
		// 检查OpenGL版本/扩展是否支持SPIR-V
		if (!CheckSpirvSupport()) {
			std::cerr << "ERROR::SHADER::SPIRV_NOT_SUPPORTED" << std::endl;
			return;
		}
		
		// 读取SPIR-V二进制文件
		std::vector<char> vertex_data = ReadBinaryFile(vertex_spv_path);
		std::vector<char> fragment_data = ReadBinaryFile(fragment_spv_path);
		
		if (vertex_data.empty() || fragment_data.empty()) {
			std::cerr << "ERROR::SHADER::SPIRV_FILE_EMPTY" << std::endl;
			return;
		}
		
		// TODO: 实现实际的SPIR-V加载逻辑
		std::cerr << "WARNING::SHADER::SPIRV_LOADING_NOT_IMPLEMENTED" << std::endl;
		std::cerr << "Vertex SPIR-V size: " << vertex_data.size() 
		<< " bytes" << std::endl;
		std::cerr << "Fragment SPIR-V size: " << fragment_data.size() 
		<< " bytes" << std::endl;
		
		program_ = 0;  // 临时设为0
	}
	
	// 检查SPIR-V支持
	static bool CheckSpirvSupport() {
		// 检查OpenGL版本（需要4.6+或ARB_gl_spirv扩展）
//		const GLubyte* version = glGetString(GL_VERSION);
//		if (!version) return false;
//		
//		int major = 0, minor = 0;
//		sscanf(reinterpret_cast<const char*>(version), "%d.%d", &major, &minor);
//		
//		if (major > 4 || (major == 4 && minor >= 6)) {
//			return true;  // OpenGL 4.6+ 原生支持
//		}
//		
//		// 检查扩展
//		const GLubyte* extensions = glGetString(GL_EXTENSIONS);
//		if (!extensions) return false;
//		
//		std::string ext_str(reinterpret_cast<const char*>(extensions));
//		return ext_str.find("GL_ARB_gl_spirv") != std::string::npos;
		return true;
	}
	
	
};

#endif  // SHADER_SHADER_H_
