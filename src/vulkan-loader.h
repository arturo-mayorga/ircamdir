#ifndef vulkan_loader_h_
#define vulkan_loader_h_

#include <Windows.h>
#include "vulkan.h"
#include <vector>

#define LIBRARY_TYPE HMODULE

namespace Vulkan
{
#define EXPORTED_VULKAN_FUNCTION(name) extern PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;

#include "ListOfVulkanFunctions.inl"

    bool LoadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const &vulkan_library);
    bool LoadGlobalLevelFunctions();

    bool CreateVulkanInstance(std::vector<char const *> const &desired_extensions, char const *const application_name, VkInstance &instance);

    bool LoadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const &enabled_extensions);
    bool EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice> &available_devices);
}

#endif