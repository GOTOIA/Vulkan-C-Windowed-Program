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

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};


//Global
SDL_Window* window;




bool isDeviceSuitable(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
std::vector<const char*> getRequiredExtensions();
bool checkValidationLayerSupport();



int main() {
	

	VkInstance instance;
	

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device=NULL;

	VkQueue graphicsQueue=NULL;

	//TODO initWindow();
	//Set SDL and SDL WINDOW
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
	//End Set SDL and SDL Window
	
	//TODO initVulkan(&instance,&physicalDevice,&device, &graphicsQueue);

	//Create Instance
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

	VkInstanceCreateInfo InstancecreateInfo = {};
	InstancecreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstancecreateInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	InstancecreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	InstancecreateInfo.ppEnabledExtensionNames = extensions.data();

	
	if (enableValidationLayers) {
		InstancecreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		InstancecreateInfo.ppEnabledLayerNames = validationLayers.data();

	
	}
	else {
		InstancecreateInfo.enabledLayerCount = 0;

		InstancecreateInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&InstancecreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

	//End instance creation

	//pickPhysicalDevice

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}


	//end pickPhysicalDevice

	//createLogicalDevice
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo DevicecreateInfo = {};
	DevicecreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	DevicecreateInfo.pQueueCreateInfos = &queueCreateInfo;
	DevicecreateInfo.queueCreateInfoCount = 1;

	DevicecreateInfo.pEnabledFeatures = &deviceFeatures;

	DevicecreateInfo.enabledExtensionCount = 0;

	if (enableValidationLayers) {
		DevicecreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		DevicecreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		DevicecreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &DevicecreateInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);

	//end createLogicalDevice
	//Main Loop

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

	//Clean All
	//TODO cleanup(device, instance);
	vkDestroyDevice(device, nullptr);


	vkDestroyInstance(instance, nullptr);

	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	

	return EXIT_SUCCESS;
}




bool isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.isComplete();
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
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

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

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

