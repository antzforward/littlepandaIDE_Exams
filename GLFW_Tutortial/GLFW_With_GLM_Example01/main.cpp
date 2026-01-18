#include <iostream>
// GLEW 使用静态链接，注意函数选项中的相关链接参数
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
glm::vec3 calculateCircumcenter(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
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
