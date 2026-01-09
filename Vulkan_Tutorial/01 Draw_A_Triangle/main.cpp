#include <bits/stdc++.h>
using namespace std;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#ifdef NDEBUG //disable DEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

/**
 * @code ""VK_LAYER_KHRONOS_validation""
 * 需要检查的Layers。
 */
const vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

VkDebugUtilsMessengerEXT debugMessenger;
/**
 * @brief 检查是否支持ValidationLayers
 * 
 * @return 
 */
bool checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	for (const char* layerName : validationLayers) {
		bool layerFound = false;
		
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		
		if (!layerFound) {
			return false;
		}
	}
	
	return true;
}
/**
 * @brief 在Instance创建后，创建Debug Messager向validation layer传递错误信息
 * 
 * @param instance 
 * @param pAllocator 
 * @param pCreateInfo 
 * @param pDebugMessenger 
 * 
 * @return 
 */
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	
	return VK_FALSE;
}

int main() {
	// 所有资源初始化为空/默认值
	GLFWwindow* window = nullptr;
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	int result = 0; // 0=成功, 其他=错误阶段
	// 用 do-while(false) + break 实现分段跳转
	do {
		// 阶段 1: 初始化 GLFW
		if (!glfwInit()) {
			result = -1;
			break;
		}
		
		// 阶段 2: 创建窗口
		window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
		if (!window) {
			result = -2;
			break;
		}
		
		// 阶段 3: 检查 Validation Layers
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			result = -3;
			break;
		}
		
		// 阶段 4: 配置 Vulkan 实例
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.apiVersion = VK_API_VERSION_1_0;
		
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			debugCreateInfo.pfnUserCallback = debugCallback;
			createInfo.pNext = &debugCreateInfo;
		}
		
		// 阶段 5: 创建 Vulkan 实例
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			result = -4;
			break;
		}
		
		// 阶段 6: 设置 Debug Messenger
		if (enableValidationLayers && 
			CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			result = -5;
			break;
		}
		
		// 阶段 7: 主循环
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
		
	} while (false);
	
	// 统一资源清理（根据 result 判断需要清理哪些资源）
	switch (result) {
		case -5: // 清理 Debug Messenger
			if (enableValidationLayers) {
				DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
			}
			[[fallthrough]]; // 继续执行下一阶段清理
		case -4: // 清理 Vulkan 实例
			if (instance != VK_NULL_HANDLE) {
				vkDestroyInstance(instance, nullptr);
			}
			[[fallthrough]];
		case -3: 
		case -2:// 清理窗口
			if (window) {
				glfwDestroyWindow(window);
			}
			[[fallthrough]];
		case -1: // 终止 GLFW
			glfwTerminate();
			break;
		default: // 成功，无需清理
			break;
	}
	
	return (result == 0) ? 0 : -1;
}
