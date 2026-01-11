#include <iostream>  // 添加缺失的头文件
#include <vector>     // 明确包含vector
#include <optional>   // 添加optional头文件
#include <string_view>
using namespace std;

//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

#ifdef NDEBUG // disable DEBUG
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
 * @brief 检查是否支持 Validation Layers
 * @return 如果所有需要的 Layers 都存在，返回 true；否则返回 false
 */
bool checkValidationLayerSupport() {
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	for (const char* layerName : validationLayers) {
		const std::string_view targetLayer(layerName);  // 转换为 string_view
		bool layerFound = false;
		
		for (const auto& layerProperties : availableLayers) {
			// 直接比较 string_view，避免调用 strcmp
			if (targetLayer == layerProperties.layerName) {
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
	cerr << "validation layer: " << pCallbackData->pMessage << endl;
	
	return VK_FALSE;
}

int main() {
	// 所有资源初始化为空/默认值
	GLFWwindow* window = nullptr;
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;  // 在主函数作用域
	VkDevice device = VK_NULL_HANDLE;  // 初始化为VK_NULL_HANDLE
	VkQueue  graphicsQueue = VK_NULL_HANDLE;  // 初始化为VK_NULL_HANDLE
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
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
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
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfo.pfnUserCallback = debugCallback;
			createInfo.pNext = &debugCreateInfo;
		} else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
		
		// 阶段 5: 创建 Vulkan 实例
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			result = -4;
			break;
		}
		
		// 阶段 6: 设置 Debug Messenger
		if (enableValidationLayers) {
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfoActual{};
			debugCreateInfoActual.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfoActual.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfoActual.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfoActual.pfnUserCallback = debugCallback;
			
			if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfoActual, nullptr, &debugMessenger) != VK_SUCCESS) {
				cout << "CreateDebugUtilsMessengerEXT: false" << endl; 
				result = -5;
				break;
			}
		}
		
		// 阶段 7：选择显卡，Pick Physical Device
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		cout << "vkEnumeratePhysicalDevices: " << deviceCount << endl; 
		if (deviceCount == 0) {
			result = -6;
			break;
		}
		
		vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
		
		optional<uint32_t> graphicsQueueFamilyIndex;
		
		for (const auto& dev : devices) {  // 使用不同的变量名，避免遮蔽
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);
			
			cout << "GPU queue family count: " << queueFamilyCount << endl;
			
			if (queueFamilyCount == 0) {
				continue;
			}
			
			// ✅ 正确：使用 resize() 设置 vector 大小
			vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());
			
			// 查找支持图形队列的队列族
			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				const auto& queueFamily = queueFamilies[i];
				
				cout << "Queue family " << i << " flags: " 
				<< queueFamily.queueFlags << " "
				<< "Has graphics: " 
				<< boolalpha << static_cast<bool>(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				<< endl;
				
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					graphicsQueueFamilyIndex = i;
					break;  // 找到第一个支持图形的队列族
				}
			}
			
			if (graphicsQueueFamilyIndex.has_value()) {
				cout << "Found suitable GPU with graphics queue family: " 
				<< graphicsQueueFamilyIndex.value() << endl;
				physicalDevice = dev;  // 赋值给外层变量
				break;  // 找到合适的物理设备
			}
		}
		
		// 检查是否真的找到了物理设备
		if (physicalDevice == VK_NULL_HANDLE) {
			cout << "failed to find a suitable GPU!" << endl;
			result = -7;
			break;
		}
		
		// 阶段 8：创建逻辑设备
		if (!graphicsQueueFamilyIndex.has_value()) {
			cout << "No graphics queue family found!" << endl;
			result = -8;
			break;
		}
		
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex.value();
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		
		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceInfo.queueCreateInfoCount = 1;
		deviceInfo.pEnabledFeatures = &deviceFeatures;
		deviceInfo.enabledExtensionCount = 0;
		
		if (enableValidationLayers) {
			deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			deviceInfo.enabledLayerCount = 0;
		}
		
		if (vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
			cout << "failed to create logical device!" << endl;
			result = -9; 
			break;
		}
		
		vkGetDeviceQueue(device, graphicsQueueFamilyIndex.value(), 0, &graphicsQueue);
		
		// 阶段 9: 主循环
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
		
	} while (false);
	
	// 统一资源清理（根据 result 判断需要清理哪些资源）
	switch (result) {
		case 0:  // 成功，清理所有资源
		case -9:  // 清理逻辑设备
		if (device != VK_NULL_HANDLE) {
			vkDestroyDevice(device, nullptr);
		}
		[[fallthrough]];  // 继续执行下一阶段清理
	case -8:
	case -7:
		case -6:  // 清理 Debug Messenger
		if (enableValidationLayers && debugMessenger != VK_NULL_HANDLE) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		[[fallthrough]];  // 继续执行下一阶段清理
		case -5:  // 清理 Vulkan 实例
	case -4:
		if (instance != VK_NULL_HANDLE) {
			vkDestroyInstance(instance, nullptr);
		}
		[[fallthrough]];
		case -3: 
		case -2:  // 清理窗口
		if (window) {
			glfwDestroyWindow(window);
		}
		[[fallthrough]];
		case -1:  // 终止 GLFW
		glfwTerminate();
		break;
	}
	
	return result;
}
