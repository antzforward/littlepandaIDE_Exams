#include <iostream>
#include <vector>
#include <string_view>
#include <unordered_map>
using namespace std;
// GLEW 使用静态链接，注意函数选项中的相关链接参数
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void PrintVersionIfo();
static std::string GetBinaryFormatName(GLenum format);
static std::string GuessFormatByExtension(GLenum format);
static std::string GetFormatDescription(GLenum format);
static bool HasExtension(const std::string& extension);
static glm::vec3 calculateCircumcenter(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
// 窗口大小
const GLuint WIDTH = 800, HEIGHT = 600;

// main函数，程序从这里开始执行
int main()
{
    // 初始化GLFW
    glfwInit();
    // 设置GLFW必要选项
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建GLFW窗口对象
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // 设置必要的回调函数
    glfwSetKeyCallback(window, key_callback);
	
    // 让GLEW使用现代方式来获取函数指针和扩展
    glewExperimental = GL_TRUE;
    // 初始化GLEW，设置OpenGL函数指针
    glewInit();
	
	//打印信息
	PrintVersionIfo();
	
    // 设置视图尺寸
    glViewport(0, 0, WIDTH, HEIGHT);

    // 创建两个着色器程序：一个用于填充三角形，一个用于线框
	Shader fillShader("shader.vs", "shader.fs");
	Shader wireframeShader("shader.vs", "shader_wireframe.fs");


    // 一维数组，每六个代表一个顶点属性，前三个代表位置属性，后三个代表颜色属性
    GLfloat vertices[] = {
        // Positions         // Colors
        -1.0f, -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
         0.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
        1.0f,  -1.0f, 0.0f,   0.0f, 0.0f, 1.0f   // Top 
    };
    GLuint VBO, VAO;//声明顶点缓冲，声明顶点数组用于管理顶点数据
    glGenVertexArrays(1, &VAO);//创建顶点数组，返回一个独一无二的整数，标识数组
    glGenBuffers(1, &VBO);//创建顶点缓冲，返回一个独一无二的整数，标识缓冲区
    
    glBindVertexArray(VAO);//绑定顶点数组
    glBindBuffer(GL_ARRAY_BUFFER, VBO);//绑定顶点缓冲
	//指定顶点数组的数据源为vertices，第四个参数代表显卡如何管理给定的数据，GL_STATIC_DRWA代表几乎不会改变
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 指定顶点属性的解析方式。即，如何从顶点缓冲获取相应的顶点属性和相应的颜色属性。或者说，顶点着色器中如何知道去哪个顶点属性分量重着色呢
	//对每一个顶点而言，属性有2种，一是位置属性，而是颜色属性，因此每六个浮点数决定了一个顶点的位置和颜色

	//顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)，因此第一个参数，代表属性分量的索引
	//参数二：顶点位置属性的维度，参数三：属性向量的数据类型，参数四:是否标准化；参数五，顶点位置属性的总字节长度，参数六：在缓冲数组中的偏移量，即起始位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);//启用属性0，因为默认是禁用的

    // 参数一，对应顶点着色器中的layout (location = 1) in vec3 color;参数六：说明颜色属性的偏移量在三个浮点数后，与上文vertices一致
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);//启用属性1.

	//顶点数组对象(Vertex Array Object, VAO)的好处就是，当配置顶点属性指针时，你只需要将上面的代码调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。如下文循环中的绑定再解绑
    glBindVertexArray(0); // 解绑 VAO
	
	
	// 使用GLM计算MVP矩阵
	// 方法1：精确计算三角形中心
	// 三角形的质心 = (顶点1 + 顶点2 + 顶点3) / 3
	// 外接圆的圆心 公式： D=2[x1(y2-y3)+x2(y3-y1)+x3(y1-y2)]
	int offset = 0;
	glm::vec3 A(vertices[offset+0],vertices[offset+1],vertices[offset+2]);
	offset = 6;
	glm::vec3 B(vertices[offset+0],vertices[offset+1],vertices[offset+2]);
	offset = 12;
	glm::vec3 C(vertices[offset+0],vertices[offset+1],vertices[offset+2]);
	glm::vec3 circumcenter = calculateCircumcenter(A, B, C);
	// 视图矩阵：假设摄像机在原点，看向z轴负方向，上向量为y轴
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f,  4.0f), 
								 glm::vec3(0.0f, 0.0f, 0.0f), 
								 glm::vec3(0.0f, 1.0f, 0.0f));
	// 投影矩阵：透视投影
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH/HEIGHT, 0.1f, 100.0f);
	
	// 获取uniform的位置
	auto fillMvpLoc = glGetUniformLocation(fillShader.Program, "mvp");
	auto wireMvpLoc = glGetUniformLocation(wireframeShader.Program, "mvp");
	auto wireColorLoc = glGetUniformLocation(wireframeShader.Program, "wireColor"); 
	//可以提前设置，也可以在loop里面设置，静态变量就设置一次好了。
	wireframeShader.Use();
	if( wireColorLoc != GL_INVALID_INDEX ){
		glUniform3f(wireColorLoc,1.0f,1.0f,1.0f);
	}
    // 游戏主循环
    while (!glfwWindowShouldClose(window))
    {
        // 检查事件，调用相应的回调函数，如下文的key_callback函数
        glfwPollEvents();

        // 渲染
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// 随时间旋转
		float time = glfwGetTime();
		
		// ********** 绘制旋转的填充三角形 **********
		fillShader.Use();
		
		// 计算旋转模型矩阵
		glm::mat4 fillModel = glm::mat4(1.0f);
		fillModel = glm::translate(fillModel, circumcenter);      // 平移到原点
		fillModel = glm::rotate(fillModel, time, glm::vec3(0.0f, 0.0f, 1.0f));  // 旋转
		fillModel = glm::translate(fillModel, -circumcenter);     // 平移回原位
		
		glm::mat4 fillMvp = projection * view * fillModel;
		glUniformMatrix4fv(fillMvpLoc, 1, GL_FALSE, glm::value_ptr(fillMvp));
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		// ********** 绘制原始三角形的边框 **********
		wireframeShader.Use();
//		if( wireColorLoc != -1 ){
//			glUniform3f(wireColorLoc,1.0f,1.0f,1.0f);
//		}
		// 原始三角形的模型矩阵（不旋转）
		glm::mat4 wireModel = glm::mat4(1.0f);
		glm::mat4 wireMvp = projection * view * wireModel;
		glUniformMatrix4fv(wireMvpLoc, 1, GL_FALSE, glm::value_ptr(wireMvp));
		
		// 设置线框模式和线宽
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(3.0f);  // 设置线宽
		
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		// 恢复填充模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
    }
    // 正确释放所有不再需要的资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // 终止GLFW，清理所有GLFW使用的资源
    glfwTerminate();
    return 0;
}


// 当键盘按键被按下/释放时，该函数被调用
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// 计算三角形外接圆圆心（外心）
glm::vec3 calculateCircumcenter(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) {
	// 使用重心坐标法计算外心
	float a = glm::length(B - C);
	float b = glm::length(A - C);
	float c = glm::length(A - B);
	
	// 计算重心坐标系数
	float a2 = a * a * (b * b + c * c - a * a);
	float b2 = b * b * (a * a + c * c - b * b);
	float c2 = c * c * (a * a + b * b - c * c);
	
	glm::vec3 circumcenter = (a2 * A + b2 * B + c2 * C) / (a2 + b2 + c2);
	return circumcenter;
}
void PrintVersionIfo(){
	// 获取 OpenGL 版本信息
	std::cout << "\n=== OpenGL 版本信息 ===" << std::endl;
	
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	
	std::cout << "OpenGL 版本: " << version << std::endl;
	std::cout << "GLSL 版本: " << glslVersion << std::endl;
	std::cout << "渲染器: " << renderer << std::endl;
	
	// 获取数值版本
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	std::cout << "主版本: " << major << ".次版本: " << minor << std::endl;
	
	// 获取厂商信息
	std::cout << "\n=== 厂商信息 ===" << std::endl;
	
	const GLubyte* vendor = glGetString(GL_VENDOR);
	std::cout << "厂商: " << vendor << std::endl;
	// 获取扩展列表
	std::cout << "\n=== 扩展支持 ===" << std::endl;
	
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	std::cout << "支持的扩展数量: " << numExtensions << std::endl;

	
	// 只显示与着色器相关的扩展
	std::vector<std::string> shaderExtensions = {
		"GL_ARB_get_program_binary",
		"GL_ARB_gl_spirv",
		"GL_ARB_spirv_extensions",
		"GL_KHR_parallel_shader_compile",
		"GL_ARB_separate_shader_objects",
		"GL_ARB_program_interface_query"
	};
	
	for (const auto& ext : shaderExtensions)
	{
		if (HasExtension(ext))
		{
			std::cout << "✓ " << ext << std::endl;
		}
	}
	// 查询着色器二进制支持信息
	std::cout << "\n=== 着色器二进制支持 ===" << std::endl;
	
	// 检查是否支持程序二进制
	if (HasExtension("GL_ARB_get_program_binary"))
	{
		std::cout << "支持程序二进制加载/保存" << std::endl;
		
		// 查询支持的二进制格式数量
		GLint numFormats = 0;
		glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
		std::cout << "支持的二进制格式数量: " << numFormats << std::endl;
		
		if (numFormats > 0)
		{
			std::vector<GLint> formats(numFormats);
			glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats.data());
			
			std::cout << "支持的二进制格式:" << std::endl;
			for (GLint format : formats)
			{
				std::string formatName = GetBinaryFormatName(format);
				std::cout << "  - 0x" << std::hex << format << std::dec 
				<< " (" << formatName << ")" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "不支持程序二进制" << std::endl;
	}
	
	// 检查 SPIR-V 支持
	if (HasExtension("GL_ARB_gl_spirv"))
	{
		std::cout << "支持 SPIR-V 着色器" << std::endl;
		
		// 查询 SPIR-V 版本
		GLint spirvVersionMajor = 0, spirvVersionMinor = 0;
		glGetIntegerv(GL_SPIR_V_EXTENSIONS, &spirvVersionMajor); // 注意：这个查询可能不正确
		// 实际中，GL_ARB_gl_spirv 规范定义了特定的查询方式
		
		std::cout << "SPIR-V 版本: 1." << spirvVersionMinor << std::endl;
	}
	else
	{
		std::cout << "不支持 SPIR-V" << std::endl;
	}
	
	// 查询各种限制
	std::cout << "\n=== 系统限制 ===" << std::endl;
	
	struct LimitInfo {
		GLenum pname;
		const char* name;
	};
	
	std::vector<LimitInfo> limits = {
		{GL_MAX_TEXTURE_SIZE, "最大纹理尺寸"},
		{GL_MAX_VERTEX_ATTRIBS, "最大顶点属性数量"},
		{GL_MAX_VERTEX_UNIFORM_COMPONENTS, "顶点着色器uniform组件数"},
		{GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, "片段着色器uniform组件数"},
		{GL_MAX_VARYING_COMPONENTS, "最大varying组件数"},
		{GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, "顶点着色器纹理单元数"},
		{GL_MAX_TEXTURE_IMAGE_UNITS, "片段着色器纹理单元数"},
		{GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "总纹理单元数"},
		{GL_MAX_UNIFORM_BLOCK_SIZE, "最大uniform块大小"},
		{GL_MAX_PROGRAM_TEXEL_OFFSET, "最大程序纹理偏移"},
		{GL_MIN_PROGRAM_TEXEL_OFFSET, "最小程序纹理偏移"}
	};
	
	for (const auto& limit : limits)
	{
		GLint value;
		glGetIntegerv(limit.pname, &value);
		std::cout << limit.name << ": " << value << std::endl;
	}
}
// 获取二进制格式名称（通过已知常量映射）
static std::string GetBinaryFormatName(GLenum format)
{
	// 这些格式常量来自不同的 OpenGL 扩展规范
	switch (format)
	{
		// 标准格式
		case 0x8FBA: return "GL_PROGRAM_BINARY_FORMAT_ARB";  // ARB_get_program_binary
		case 0x8FBB: return "GL_PROGRAM_BINARY_FORMAT_SPIR_V_ARB";  // SPIR-V
		
		// 厂商特定格式
		case 0x87FC: return "GL_PROGRAM_BINARY_FORMAT_MESA";  // Mesa
		case 0x96B0: return "GL_PROGRAM_BINARY_FORMAT_POWERVR";  // PowerVR/IMG
		case 0x913F: return "GL_PROGRAM_BINARY_FORMAT_NVIDIA";  // NVIDIA
		case 0x8C40: return "GL_PROGRAM_BINARY_FORMAT_AMD";  // AMD
		case 0x93B0: return "GL_PROGRAM_BINARY_FORMAT_INTEL";  // Intel
		case 0x9555: return "GL_PROGRAM_BINARY_FORMAT_IMG";  // IMG (PowerVR)
		
		// Khronos 标准格式
		case 0x93A0: return "GL_PROGRAM_BINARY_FORMAT_KHR";  // Khronos
		
		// GL 4.5+ 标准
		case 0x9288: return "GL_SHADER_BINARY_FORMAT_SPIR_V";  // GL 4.6 SPIR-V
		case 0x9289: return "GL_SHADER_BINARY_FORMAT_SPIR_V_ARB";  // ARB_spirv_extensions
		
		// Vulkan 互操作性
		case 0x95B8: return "GL_SHADER_BINARY_FORMAT_VULKAN";  // EXT_external_objects
		
		// 更多可能的格式
		case 0x8A4F: return "GL_PROGRAM_BINARY_RETRIEVABLE_HINT";
		case 0x8741: return "GL_PROGRAM_BINARY_FORMATS";
		
	default:
		// 尝试通过扩展字符串来识别未知格式
		return GuessFormatByExtension(format);
	}
}

// 通过查询支持的扩展来猜测格式
static std::string GuessFormatByExtension(GLenum format)
{
	std::stringstream ss;
	ss << "0x" << std::hex << format << std::dec;
	
	// 检查支持的扩展
	if (HasExtension("GL_ARB_get_program_binary") && format == 0x8FBA)
		return "GL_PROGRAM_BINARY_FORMAT_ARB";
	
	if (HasExtension("GL_ARB_gl_spirv") && format == 0x8FBB)
		return "GL_PROGRAM_BINARY_FORMAT_SPIR_V_ARB";
	
	if (HasExtension("GL_MESA_get_program_binary") && format == 0x87FC)
		return "GL_PROGRAM_BINARY_FORMAT_MESA";
	
	if (HasExtension("GL_IMG_binary_shader") && format == 0x96B0)
		return "GL_PROGRAM_BINARY_FORMAT_POWERVR";
	
	if (HasExtension("GL_NV_gpu_program5") && format == 0x913F)
		return "GL_PROGRAM_BINARY_FORMAT_NVIDIA";
	
	if (HasExtension("GL_AMD_program_binary_Z400") && format == 0x8C40)
		return "GL_PROGRAM_BINARY_FORMAT_AMD";
	
	if (HasExtension("GL_INTEL_binary_shader") && format == 0x93B0)
		return "GL_PROGRAM_BINARY_FORMAT_INTEL";
	
	// 无法识别的格式
	return "未知格式 (0x" + std::to_string(static_cast<unsigned int>(format)) + ")";
}

// 获取格式描述
static std::string GetFormatDescription(GLenum format)
{
	switch (format)
	{
		case 0x8FBA: return "ARB_get_program_binary 标准格式，驱动私有二进制";
		case 0x8FBB: return "SPIR-V 格式，跨平台中间表示";
		case 0x87FC: return "Mesa 开源驱动格式";
		case 0x96B0: return "PowerVR/IMG 移动GPU格式";
		case 0x913F: return "NVIDIA 私有二进制格式";
		case 0x8C40: return "AMD 私有二进制格式";
		case 0x93B0: return "Intel GPU 格式";
		case 0x9555: return "IMG/PowerVR 替代格式";
		case 0x93A0: return "Khronos 标准二进制格式";
		case 0x9288: return "标准 SPIR-V 着色器二进制";
		case 0x9289: return "ARB SPIR-V 扩展格式";
		case 0x95B8: return "Vulkan 互操作格式";
		default: return "厂商特定的私有二进制格式";
	}
}
static bool HasExtension(const std::string& extension)
{
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	
	for (GLint i = 0; i < numExtensions; ++i)
	{
		const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
		if (extension == ext)
			return true;
	}
	return false;
}
