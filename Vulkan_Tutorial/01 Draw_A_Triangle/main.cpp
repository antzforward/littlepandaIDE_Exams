#include <iostream>  // 添加缺失的头文件
#include <vector>     // 明确包含vector
#include <optional>   // 添加optional头文件
#include <string_view>
#include <set>
#include <limits>
#include <algorithm>
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
/**
 * @code VK_KHR_SWAPCHAIN_EXTENSION_NAME
 * 需要检查的Device Extension Support
 */
const vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;  // 在主函数作用域
	VkDevice device = VK_NULL_HANDLE;  // 初始化为VK_NULL_HANDLE
	VkQueue  graphicsQueue = VK_NULL_HANDLE;  // 初始化为VK_NULL_HANDLE
	VkQueue  presentQueue = VK_NULL_HANDLE;
	
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	vector<VkImageView> swapChainImageViews;
	vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	
	int result = 0; // 0=成功, 其他=错误阶段
	
	// 用 do-while(false) + break 实现分段跳转
	do {
		// 阶段 1: 初始化 GLFW
		if (!glfwInit()) {
			result = -1;
			break;
		}
		
		// 阶段 2: 创建窗口 - 添加窗口提示，不要创建OpenGL上下文占据window
		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );//关键：告诉 GLFW 不要创建 OpenGL 上下文
		glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE);//可选：禁止调整大小
		window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
		if (!window) {
			result = -2;
			break;
		}
		
		// 阶段 3: 检查 Validation Layers
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			result = -2;
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
		appInfo.pApplicationName = "Vulkan App";
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
			result = -3;
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
				result = -4;
				break;
			}
		}
		//阶段 7：创建 Surface
		VkResult surfaceResult = glfwCreateWindowSurface(instance, window, nullptr, &surface);
		if( surfaceResult != VK_SUCCESS )
		{
			cout<<"failed to create window surface!";
			switch (surfaceResult) {
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				cout <<"VK_ERROR_OUT_OF_HOST_MEMORY"<<endl;
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				cout <<"VK_ERROR_OUT_OF_DEVICE_MEMORY"<<endl;
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				cout <<"VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"<<endl;
				break;
			default:
				cout << "Unknown error (" << surfaceResult << ")" << endl;
				break;
			}
			result = -5;
			break;
		}
		// 阶段 8：选择显卡，Pick Physical Device
		// 注意图形队列和呈现队列可以是不同的队列族
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		cout << "vkEnumeratePhysicalDevices: " << deviceCount << endl; 
		if (deviceCount == 0) {
			result = -5;
			break;
		}
		
		vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
		
		optional<uint32_t> graphicsQueueFamilyIndex;
		optional<uint32_t> presentQueueFamilyIndex;
		vector<VkQueueFamilyProperties> queueFamilies{};
		vector<VkExtensionProperties> availableExtensions{};
		for (const auto& dev : devices) {  // 使用不同的变量名，避免遮蔽
			//关键，防止选择了不同的dev中的familyIndex
			graphicsQueueFamilyIndex.reset();
			presentQueueFamilyIndex.reset();
			
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);
			cout << "GPU queue family count: " << queueFamilyCount << endl;
			if (queueFamilyCount == 0) {
				continue;
			}
			//检查dev 是否支持swap chain
			uint32_t extensionCount =0;
			vkEnumerateDeviceExtensionProperties(dev, nullptr,&extensionCount,nullptr);
			availableExtensions.resize( extensionCount );
			vkEnumerateDeviceExtensionProperties( dev, nullptr, &extensionCount, availableExtensions.data());
			set<string_view> requiredExtensions( deviceExtensions.begin(), deviceExtensions.end());
			for(const auto& extension:availableExtensions){
				requiredExtensions.erase(string_view(extension.extensionName));
			}
			
			if( !requiredExtensions.empty() ){
				cout<<"the follow device extension is not support";
				for(const auto& strz:requiredExtensions){
					cout<< strz << " ";
				}
				cout<<endl;
				continue;
			}
			
			// ✅ 正确：使用 resize() 设置 vector 大小
			queueFamilies.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());
			
			// 查找支持图形队列的队列族
			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				const auto& queueFamily = queueFamilies[i];
				
				cout << "Queue family " << i << " flags: " 
				<< queueFamily.queueFlags << " "
				<< "Has graphics: " 
				<< boolalpha << static_cast<bool>(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				<< endl;
				if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					graphicsQueueFamilyIndex = i;
					
				}
				
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface,&presentSupport);
				if(presentSupport )
				{
					presentQueueFamilyIndex = i;
				}
				
				
				if (graphicsQueueFamilyIndex.has_value() && presentQueueFamilyIndex.has_value() ) {
					break;
				}
			}
			
			if (graphicsQueueFamilyIndex.has_value() && presentQueueFamilyIndex.has_value() ) {
				cout << "Found suitable GPU with graphics queue family: " 
				<< graphicsQueueFamilyIndex.value() 
				<< endl
				<< "Found suitable GPU with Present Queue Family:"
				<< presentQueueFamilyIndex.value()
				<< endl;
				physicalDevice = dev;  // 赋值给外层变量
				break;  // 找到合适的物理设备
			}
		}
		
		// 检查是否真的找到了物理设备
		if (physicalDevice == VK_NULL_HANDLE) {
			cout << "failed to find a suitable GPU!" << endl;
			result = -5;
			break;
		}
		
		// 阶段 9：创建逻辑设备
		if (!graphicsQueueFamilyIndex.has_value() ) {
			cout << "No graphics queue family found!" << endl;
			result = -5;
			break;
		}
		if (!presentQueueFamilyIndex.has_value() ) {
			cout << "No present queue family found!" << endl;
			result = -5;
			break;
		}
		
		//创建带多个Graphics Queue和Present Queue的逻辑device
		//注意两个queue的family可能是同一族的，所以要注意设置
		vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		set<uint32_t> uniqueQueueFamilies = { presentQueueFamilyIndex.value(),graphicsQueueFamilyIndex.value()};
		float queuePriority = 1.0f;
		for( uint32_t queueFamily:uniqueQueueFamilies){
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex.value();
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
		
		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceInfo.pEnabledFeatures = &deviceFeatures;
		deviceInfo.enabledExtensionCount = 0;
		deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
		
		if (enableValidationLayers) {
			deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			deviceInfo.enabledLayerCount = 0;
		}
		
		if (vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
			cout << "failed to create logical device!" << endl;
			result = -6; 
			break;
		}
		
		vkGetDeviceQueue(device, graphicsQueueFamilyIndex.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, presentQueueFamilyIndex.value(), 0, &presentQueue);
		// 阶段 10：Swap Chain
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,surface,&surfaceCapabilities);
		
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
		vector<VkSurfaceFormatKHR> formats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
		
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
		vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
		
		VkSurfaceFormatKHR chosenSwapSurfaceFormat = formats[0];//default
		for(int i=0;i<formats.size();i++){
			if( formats[i].format == VK_FORMAT_R8G8B8A8_SRGB
				&& formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR
				)
			{
				chosenSwapSurfaceFormat = formats[i];
				break;
			}
		}
		VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for(int i=0;i<presentModes.size();i++){
			if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR )
			{
				chosenPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
		}
		
		VkExtent2D chosenExtent2D = surfaceCapabilities.currentExtent;
		if( surfaceCapabilities.currentExtent.width == numeric_limits<uint32_t>::max())
		{
			int w,h;
			glfwGetFramebufferSize(window, &w, &h);
			chosenExtent2D.width = clamp( static_cast<uint32_t>(w), surfaceCapabilities.minImageExtent.width,surfaceCapabilities.maxImageExtent.width );
			chosenExtent2D.height = clamp(static_cast<uint32_t>(h), surfaceCapabilities.minImageExtent.height,surfaceCapabilities.maxImageExtent.height );
		}
		
		uint32_t imageCount = surfaceCapabilities.minImageCount + 1;//至少大于1个
		if( surfaceCapabilities.maxImageCount >0 && imageCount>surfaceCapabilities.maxImageCount ){
			imageCount = surfaceCapabilities.maxImageCount;
		}
		
		VkSwapchainCreateInfoKHR swapChainInfo{};
		swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainInfo.surface = surface;
		
		swapChainInfo.minImageCount = imageCount;
		swapChainInfo.imageFormat = chosenSwapSurfaceFormat.format;
		swapChainInfo.imageColorSpace = chosenSwapSurfaceFormat.colorSpace;
		swapChainInfo.imageExtent = chosenExtent2D;
		swapChainInfo.imageArrayLayers = 1;
		swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		
		uint32_t queueFamilyIndices[]={graphicsQueueFamilyIndex.value(),presentQueueFamilyIndex.value()};
		//考虑当graphics family和present family不同的时候
		if( graphicsQueueFamilyIndex.value() != presentQueueFamilyIndex.value() ){
			swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;//并行模式
			swapChainInfo.queueFamilyIndexCount = 2;
			swapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
		}else{
			swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		
		swapChainInfo.preTransform = surfaceCapabilities.currentTransform;
		swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainInfo.presentMode = chosenPresentMode;
		swapChainInfo.clipped = VK_TRUE;
		
		swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
		
		if( vkCreateSwapchainKHR(device, &swapChainInfo, nullptr, &swapChain) != VK_SUCCESS ){
			cout<<"failed to create swap chain!"<<endl;
			result = -7;
			break;
		}
		
		//阶段10：创建ImageView for SwapChain
		vkGetSwapchainImagesKHR( device, swapChain, &imageCount,nullptr);
		swapChainImages.resize( imageCount );
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
		swapChainImageFormat = chosenSwapSurfaceFormat.format;
		swapChainExtent = chosenExtent2D;
		
		for(size_t i=0;i<swapChainImages.size();i++){
			VkImageViewCreateInfo imageViewInfo{};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = swapChainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = swapChainImageFormat;
			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;
			VkImageView imageView = VK_NULL_HANDLE;
			if(vkCreateImageView(device,&imageViewInfo,nullptr, &imageView) != VK_SUCCESS){
				cout<<"failed to create image views!"<<endl;
				result = -8;
				break;
			}
			swapChainImageViews.push_back(imageView );
			
		}
		if( swapChainImageViews.size()!= swapChainImages.size()){
			result = -8;
			break;
		}
		// 阶段 Loop: 主循环
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
		
	} while (false);
	
	// 统一资源清理（根据 result 判断需要清理哪些资源）
	switch (result) {
	case -8:
		for(size_t i=0;i<swapChainImageViews.size();i++){
			vkDestroyImageView(device, swapChainImageViews[i],nullptr);
		}
		swapChainImageViews.clear();
	case -7://清理swapchain
		if( swapChain != VK_NULL_HANDLE ){
			vkDestroySwapchainKHR(device, swapChain,nullptr);
		}
		[[fallthrough]];  // 继续执行下一阶段清理
	case -6:// 清理逻辑设备
		if (surface != VK_NULL_HANDLE) {
			vkDestroyDevice( device, nullptr);
		}
		[[fallthrough]];  // 继续执行下一阶段清理
	case -5: // 清理窗口的surface
		if( surface != VK_NULL_HANDLE ){
			vkDestroySurfaceKHR(instance, surface, nullptr);
		}
		[[fallthrough]];  // 继续执行下一阶段清理
	case -4:  // 清理 Debug Messenger
		if (enableValidationLayers && debugMessenger != VK_NULL_HANDLE) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		[[fallthrough]];  // 继续执行下一阶段清理
	case -3:
		if (instance != VK_NULL_HANDLE) {
			vkDestroyInstance(instance, nullptr);
		}
		[[fallthrough]];
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
