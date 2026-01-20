#ifndef SHADER_SHADER_H_
#define SHADER_SHADER_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Shader {
private:
	GLuint program_;
	bool is_spirv_;
	
	//ShaderToy特定的Uniform的情况
	bool  bBindParameters;
	GLint iResolution_;
	GLint iTime_;
	GLint iTimeDelta_;
	GLint iFrame_;
	GLint iMouse_;
	GLint iChannel0_;
	GLint iChannel1_;
	GLint iChannel2_;
	GLint iChannel3_;
	
	// 禁止复制构造和赋值
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader()
	{
		program_     = GL_INVALID_INDEX;
		is_spirv_ 	 = false;
		iResolution_ = GL_INVALID_INDEX;
		iTime_		 = GL_INVALID_INDEX;
		iTimeDelta_  = GL_INVALID_INDEX;
		iFrame_		 = GL_INVALID_INDEX;
		iMouse_		 = GL_INVALID_INDEX;
		iChannel0_	 = GL_INVALID_INDEX;
		iChannel1_	 = GL_INVALID_INDEX;
		iChannel2_	 = GL_INVALID_INDEX;
		iChannel3_	 = GL_INVALID_INDEX;
		bBindParameters = false;
	}
	
public:
	// ========== 构造函数 ==========
	
	// 从GLSL字符串编译
	Shader(const std::string& vertex_code, const std::string& fragment_code, 
		   bool from_file = false) 
	: Shader() {
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
	: Shader() {
		is_spirv_ = true;
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
		if (program_ != GL_INVALID_INDEX) {
			glDeleteProgram(program_);
		}
	}
	
	// ========== 公有方法 ==========
	
	// 使用着色器
	void Use() const {
		if (program_ != GL_INVALID_INDEX) {
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
	bool IsValid() const { return program_ != GL_INVALID_INDEX; }
	
	// 获取Uniform位置
	GLint GetUniformLocation(const std::string& name) const {
		if (program_ == GL_INVALID_INDEX) return GL_INVALID_INDEX;
		return glGetUniformLocation(program_, name.c_str());
	}
	
	// 设置Uniform的便捷函数
	void SetUniform(const std::string& name, int value) const {
		GLint loc = GetUniformLocation(name);
		if (loc != GL_INVALID_INDEX) glUniform1i(loc, value);
	}
	
	void SetUniform(const std::string& name, float value) const {
		GLint loc = GetUniformLocation(name);
		if (loc != GL_INVALID_INDEX) glUniform1f(loc, value);
	}
	
	void SetUniform(const std::string& name, float x, float y) const {
		GLint loc = GetUniformLocation(name);
		if (loc != GL_INVALID_INDEX) glUniform2f(loc, x, y);
	}
	
	void SetUniform(const std::string& name, float x, float y, float z) const {
		GLint loc = GetUniformLocation(name);
		if (loc != GL_INVALID_INDEX ) glUniform3f(loc, x, y, z);
	}
	void SetUniform(const std::string& name, float x, float y, float z,float w) const {
		GLint loc = GetUniformLocation(name);
		if (loc != GL_INVALID_INDEX ) glUniform4f(loc, x, y, z,w);
	}
	void SetUniform(const std::string& name, const glm::mat4& mat) const {
		GLint loc = GetUniformLocation(name);
		if (loc != GL_INVALID_INDEX ) glUniformMatrix4fv(loc,1, GL_FALSE, glm::value_ptr(mat));
	}
	
	
	// 设置Uniform的便捷函数
	void SetUniform(GLint loc, int value) const {
		if (loc != GL_INVALID_INDEX) glUniform1i(loc, value);
	}
	
	void SetUniform(GLint loc, float value) const {
		if (loc != GL_INVALID_INDEX) glUniform1f(loc, value);
	}
	
	void SetUniform(GLint loc, float x, float y) const {
		if (loc != GL_INVALID_INDEX) glUniform2f(loc, x, y);
	}
	
	void SetUniform(GLint loc,float x, float y, float z) const {
		if (loc != GL_INVALID_INDEX ) glUniform3f(loc, x, y, z);
	}
	void SetUniform(GLint loc,float x, float y, float z,float w) const {
		if (loc != GL_INVALID_INDEX ) glUniform4f(loc, x, y, z,w);
	}
	void SetUniform(GLint loc,const glm::mat4& mat) const {
		if (loc != GL_INVALID_INDEX ) glUniformMatrix4fv(loc,1, GL_FALSE, glm::value_ptr(mat));
	}
	
	// ====== shader toy-like =======
	void SetShaderToyUniforms(float time, float delta_time, int frame,
							  const glm::vec2& resolution,
							  const glm::vec4& mouse)const{
		if(program_ == GL_INVALID_INDEX)
			return;
		SetUniform(iTime_, time);	
		SetUniform(iTimeDelta_, delta_time);
		SetUniform(iFrame_, frame);
		SetUniform(iResolution_, resolution.x,resolution.y);
		SetUniform(iMouse_, mouse.x,mouse.y, mouse.x,mouse.z);	
	}
	
	void BindShaderToyUniforms()
	{
		if(program_ == GL_INVALID_INDEX)
			return;
		if(!bBindParameters){
			iTime_ = GetUniformLocation("iTime");
			iTimeDelta_ = GetUniformLocation("iTimeDelta");
			iFrame_	= GetUniformLocation("iFrame");
			iResolution_ = GetUniformLocation("iResolution");
			iMouse_ = GetUniformLocation("iMouse");
			bBindParameters = true;
		}
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
	
	// 加载SPIR-V着色器，检查放外面
	// 加载SPIR-V着色器
	void LoadSpirV(const std::string& vertex_spv_path, 
				   const std::string& fragment_spv_path) {
		
		// 读取SPIR-V二进制文件
		std::vector<char> vertex_data = ReadBinaryFile(vertex_spv_path);
		std::vector<char> fragment_data = ReadBinaryFile(fragment_spv_path);
		
		if (vertex_data.empty() || fragment_data.empty()) {
			std::cerr << "ERROR::SHADER::SPIRV_FILE_EMPTY" << std::endl;
			return;
		}
		
		std::cout << "Loaded vertex SPIR-V: " << vertex_data.size() << " bytes" << std::endl;
		std::cout << "Loaded fragment SPIR-V: " << fragment_data.size() << " bytes" << std::endl;
		
		// 创建着色器对象
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		
		// 设置SPIR-V二进制数据
		glShaderBinary(1, &vertex, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
					   vertex_data.data(), static_cast<GLsizei>(vertex_data.size()));
		glShaderBinary(1, &fragment, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
					   fragment_data.data(), static_cast<GLsizei>(fragment_data.size()));
		
		// 特殊化着色器（编译SPIR-V）
		const char* entry_point = "main";
		
		// 特殊化顶点着色器
		glSpecializeShader(vertex, entry_point, 0, nullptr, nullptr);
		
		// 检查顶点着色器特殊化状态
		GLint success;
		GLchar info_log[1024];
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, sizeof(info_log), nullptr, info_log);
			std::cerr << "ERROR::SHADER::VERTEX::SPIRV_SPECIALIZATION_FAILED\n" 
			<< info_log << std::endl;
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			return;
		} else {
			// 即使成功也获取信息日志，可能有警告
			glGetShaderInfoLog(vertex, sizeof(info_log), nullptr, info_log);
			if (std::strlen(info_log) > 0) {
				std::cout << "Vertex shader info: " << info_log << std::endl;
			}
		}
		
		// 特殊化片段着色器
		glSpecializeShader(fragment, entry_point, 0, nullptr, nullptr);
		
		// 检查片段着色器特殊化状态
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, sizeof(info_log), nullptr, info_log);
			std::cerr << "ERROR::SHADER::FRAGMENT::SPIRV_SPECIALIZATION_FAILED\n" 
			<< info_log << std::endl;
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			return;
		} else {
			// 即使成功也获取信息日志，可能有警告
			glGetShaderInfoLog(fragment, sizeof(info_log), nullptr, info_log);
			if (std::strlen(info_log) > 0) {
				std::cout << "Fragment shader info: " << info_log << std::endl;
			}
		}
		
		// 创建程序并链接
		program_ = glCreateProgram();
		glAttachShader(program_, vertex);
		glAttachShader(program_, fragment);
		
		// 在链接前设置一些属性绑定（如果需要）
		// 例如：glBindAttribLocation(program_, 0, "position");
		
		glLinkProgram(program_);
		
		// 检查链接状态
		glGetProgramiv(program_, GL_LINK_STATUS, &success);
		if (!success) {
			GLsizei log_length;
			glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> program_log(log_length);
			glGetProgramInfoLog(program_, log_length, nullptr, program_log.data());
			
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" 
			<< program_log.data() << std::endl;
			
			// 获取更多调试信息
			GLint active_attributes = 0;
			GLint active_uniforms = 0;
			glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTES, &active_attributes);
			glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &active_uniforms);
			
			std::cout << "Active attributes: " << active_attributes << std::endl;
			std::cout << "Active uniforms: " << active_uniforms << std::endl;
			
			// 打印所有活动属性
			if (active_attributes > 0) {
				GLint max_length;
				glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
				std::vector<GLchar> name_buffer(max_length);
				
				for (GLint i = 0; i < active_attributes; ++i) {
					GLsizei length;
					GLint size;
					GLenum type;
					glGetActiveAttrib(program_, i, max_length, &length, &size, &type, name_buffer.data());
					std::string name(name_buffer.data(), length);
					GLint location = glGetAttribLocation(program_, name.c_str());
					std::cout << "Attribute " << i << ": " << name 
					<< " (location: " << location 
					<< ", type: " << type 
					<< ", size: " << size << ")" << std::endl;
				}
			}
			
			// 清理
			glDetachShader(program_, vertex);
			glDetachShader(program_, fragment);
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			glDeleteProgram(program_);
			program_ = 0;
			return;
		}
		
		// 验证程序（可选，用于调试）
		glValidateProgram(program_);
		glGetProgramiv(program_, GL_VALIDATE_STATUS, &success);
		if (!success) {
			GLsizei log_length;
			glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> program_log(log_length);
			glGetProgramInfoLog(program_, log_length, nullptr, program_log.data());
			std::cerr << "ERROR::SHADER::PROGRAM::VALIDATION_FAILED\n" 
			<< program_log.data() << std::endl;
		}
		
		// 成功链接后删除着色器对象
		glDetachShader(program_, vertex);
		glDetachShader(program_, fragment);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		
		std::cout << "SPIR-V shader loaded successfully (ID: " 
		<< program_ << ")" << std::endl;
		
		// 打印一些程序信息
		GLint active_attributes = 0;
		GLint active_uniforms = 0;
		glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTES, &active_attributes);
		glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &active_uniforms);
		
		std::cout << "Program " << program_ << " info:" << std::endl;
		std::cout << "  Active attributes: " << active_attributes << std::endl;
		std::cout << "  Active uniforms: " << active_uniforms << std::endl;
	}	
	
};

#endif  // SHADER_SHADER_H_
