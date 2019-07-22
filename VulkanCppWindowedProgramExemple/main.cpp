/*
 * Vulkan Windowed Program
 *
 * Copyright (C) 2016, 2018 Valve Corporation
 * Copyright (C) 2016, 2018 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /*
 Vulkan C++ Windowed Project Template
 Create and destroy a Vulkan surface on an SDL window.
 */

 // Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include <optional>
#include <iostream>
#include <vector>
#include<string>
#include<set>

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



const 
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


//Global
SDL_Window* window;
VkDebugUtilsMessengerEXT debugMessenger;



int initWindow();
void initVulkan(VkInstance *instance, VkPhysicalDevice *physicalDevice, VkDevice *device, VkQueue *graphicsQueue, VkSurfaceKHR surface, VkBool32 *presentSupport, VkQueue *presentQueue);
void createInstance(VkInstance *instance);
void pickPhysicalDevice(VkInstance *instance, VkPhysicalDevice *physicalDevice, VkSurfaceKHR surface, VkBool32 *presentSupport);
void createLogicalDevice(VkPhysicalDevice *physicalDevice, VkDevice *device, VkQueue *graphicsQueue, VkSurfaceKHR surface, VkBool32 *presentSupport, VkQueue *presentQueue);
bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, VkBool32 *presentSupport);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, VkBool32 *presentSupport);
std::vector<const char*> getRequiredExtensions();
bool checkValidationLayerSupport();
void sdlCleanUp(SDL_Window* window);
void cleanup(VkDevice device, VkInstance instance);
VkResult CreateDebugUtilsMessengerEXT(VkInstance *instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
void setupDebugMessenger(VkInstance *instance);
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
int createSurface(SDL_Window* window, VkInstance instance, VkSurfaceKHR *surface);


int main() {


	VkInstance instance;


	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = NULL;

	VkQueue graphicsQueue = NULL;

	//Init SDL && SDL Window
	initWindow();

	VkSurfaceKHR surface=NULL;
	VkBool32 presentSupport = false;

	VkQueue presentQueue;

	//Init Vulkan
	initVulkan(&instance, &physicalDevice, &device, &graphicsQueue, surface, &presentSupport,&presentQueue);

	//TODO queue presentation
	


	

	
	//MainLoop
	// Poll for user input.
	bool stillRunning = true;
	while (stillRunning) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			switch (event.type) {

			case SDL_QUIT:
				stillRunning = false;
				break;

			default:
				// Do nothing.
				break;
			}
		}

		SDL_Delay(10);
	}

	
	
	vkDestroySurfaceKHR(instance,surface,nullptr);
	cleanup(device, instance);
	sdlCleanUp(window);

	return EXIT_SUCCESS;
}


int initWindow() {


	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Could not initialize SDL." << std::endl;
		return 1;
	}
	window = SDL_CreateWindow("Vulkan Window", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_VULKAN);
	if (window == NULL) {
		std::cout << "Could not create SDL window." << std::endl;
		return 1;
	}

	return 0;
}

//Init Vulkan
void initVulkan(VkInstance *instance, VkPhysicalDevice *physicalDevice, VkDevice *device, VkQueue *graphicsQueue, VkSurfaceKHR surface, VkBool32 *presentSupport, VkQueue *presentQueue) {
	createInstance(instance);
	setupDebugMessenger(instance);
	createSurface(window, *instance, &surface);
	pickPhysicalDevice(instance, physicalDevice,surface,presentSupport);
	createLogicalDevice(physicalDevice, device, graphicsQueue,surface, presentSupport, presentQueue);

}

void sdlCleanUp(SDL_Window* window) {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void cleanup(VkDevice device, VkInstance instance) {
	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);

}
//Set instance Vulkan
void createInstance(VkInstance *instance) {
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

//Select Appropriate Device(GPU compatible)
void pickPhysicalDevice(VkInstance *instance, VkPhysicalDevice *physicalDevice, VkSurfaceKHR surface, VkBool32 *presentSupport) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		
		if (isDeviceSuitable(device,surface, presentSupport)) {
			*physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

//Create logical Device who take instruction
void createLogicalDevice(VkPhysicalDevice *physicalDevice, VkDevice *device, VkQueue *graphicsQueue, VkSurfaceKHR surface, VkBool32 *presentSupport, VkQueue *presentQueue) {
	QueueFamilyIndices indices = findQueueFamilies(*physicalDevice, surface, presentSupport);


	std::vector<VkDeviceQueueCreateInfo>queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),indices.presentFamily.value() };
	

	float queuePriority = 1.0f;
	
	for(uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex =queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);

	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	

	createInfo.pEnabledFeatures = &deviceFeatures;

	
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(*physicalDevice, &createInfo, nullptr, device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(*device, indices.graphicsFamily.value(), 0, graphicsQueue);
	vkGetDeviceQueue(*device, indices.graphicsFamily.value(), 0, presentQueue);
}


//Create presentation
int createSurface(SDL_Window* window, VkInstance instance, VkSurfaceKHR *surface)
{
	if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(instance), surface)) {
		std::cout << "Could not create a Vulkan surface." << std::endl;
		return 1;
	}

	return 0;


}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> avaibleExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, avaibleExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto &extension : avaibleExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

//if device is compatible
bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, VkBool32 *presentSupport) {
	QueueFamilyIndices indices = findQueueFamilies(device,surface,presentSupport);

	bool extensionSupported = checkDeviceExtensionSupport(device);


	return indices.isComplete()&&extensionSupported;
}

//Queue of device instruction if compatible
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, VkBool32 *presentSupport) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		//provided presentation support
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

//Get sdl extension for Vulkan Instance
std::vector<const char*> getRequiredExtensions() {
	uint32_t extension_count = 0;



	if (!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, NULL)) {
		std::cout << "Could not get the number of required instance extensions from SDL." << std::endl;
		exit(1);
	}


	std::vector<const char*> extensions(extension_count);

	if (!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data())) {
		std::cout << "Could not get the names of required instance extensions from SDL." << std::endl;
		exit(1);
	}

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}
//If valid Layers
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

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}



void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

void setupDebugMessenger(VkInstance *instance) {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance *instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(*instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}