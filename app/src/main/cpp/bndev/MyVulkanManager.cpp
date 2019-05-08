#include <vulkan/vulkan.h>
#include "MatrixState3D.h"
#include "MyVulkanManager.h"
#include "../util/FileUtil.h"
#include "../util/TextureManager.h"
#include "../util/HelpFunction.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include "ThreadTask.h"
#include "../util/CameraUtil.h"
#include "../util/LightManager.h"
#include "../util/FPSUtil.h"
#include <sys/time.h>
#include "VertData.h"
void setImageLayout(VkCommandBuffer cmd, VkImage image,
                      VkImageAspectFlags aspectMask,
                      VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout)
{
    VkImageMemoryBarrier image_memory_barrier = {};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.pNext = NULL;
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = 0;
    image_memory_barrier.oldLayout = old_image_layout;
    image_memory_barrier.newLayout = new_image_layout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange.aspectMask = aspectMask;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;
    if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.srcAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (old_image_layout == VK_IMAGE_LAYOUT_PREINITIALIZED) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        image_memory_barrier.srcAccessMask =
                VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    vk::vkCmdPipelineBarrier(cmd, src_stages, dest_stages, 0, 0, NULL, 0, NULL,1, &image_memory_barrier);
}
android_app* MyVulkanManager::Android_application;
bool MyVulkanManager::loopDrawFlag=true;
std::vector<const char *>  MyVulkanManager::instanceExtensionNames;
VkInstance MyVulkanManager::instance;
uint32_t MyVulkanManager::gpuCount;
std::vector<VkPhysicalDevice> MyVulkanManager::gpus;
uint32_t MyVulkanManager::queueFamilyCount;
std::vector<VkQueueFamilyProperties> MyVulkanManager::queueFamilyprops;
uint32_t MyVulkanManager::queueGraphicsFamilyIndex;
VkQueue MyVulkanManager::queueGraphics;
uint32_t MyVulkanManager::queuePresentFamilyIndex;
std::vector<const char *> MyVulkanManager::deviceExtensionNames;
VkDevice MyVulkanManager::device;
VkCommandPool MyVulkanManager::cmdPool;
VkCommandBuffer MyVulkanManager::cmdBuffer;
VkCommandBufferBeginInfo MyVulkanManager::cmd_buf_info;
VkCommandBuffer  MyVulkanManager::cmd_bufs[1];
VkSubmitInfo MyVulkanManager::submit_info[2];
uint32_t MyVulkanManager::screenWidth;
uint32_t MyVulkanManager::screenHeight;
VkSurfaceKHR MyVulkanManager::surface;
std::vector<VkFormat> MyVulkanManager::formats;
VkSurfaceCapabilitiesKHR MyVulkanManager::surfCapabilities;
uint32_t MyVulkanManager::presentModeCount;
std::vector<VkPresentModeKHR> MyVulkanManager::presentModes;
VkExtent2D MyVulkanManager::swapchainExtent;
VkSwapchainKHR MyVulkanManager::swapChain;
uint32_t MyVulkanManager::swapchainImageCount;
std::vector<VkImage> MyVulkanManager::swapchainImages;
std::vector<VkImageView> MyVulkanManager::swapchainImageViews;
VkFormat MyVulkanManager::depthFormat;
VkFormatProperties MyVulkanManager::depthFormatProps;
VkImage MyVulkanManager::depthImage;
VkPhysicalDeviceMemoryProperties MyVulkanManager::memoryroperties;
VkDeviceMemory MyVulkanManager::memDepth;
VkImageView MyVulkanManager::depthImageView;
VkSemaphore MyVulkanManager::imageAcquiredSemaphore;
uint32_t MyVulkanManager::currentBuffer;
VkRenderPass MyVulkanManager::renderPass;
VkClearValue MyVulkanManager::clear_values[2];
VkRenderPassBeginInfo MyVulkanManager::rp_begin;
VkFence MyVulkanManager::taskFinishFence;
VkPresentInfoKHR MyVulkanManager::present;
VkFramebuffer*  MyVulkanManager::framebuffers;
VkBuffer MyVulkanManager::storageBuffer[8];
VkDeviceMemory MyVulkanManager::memStorageBuffer[8];
VkDescriptorBufferInfo MyVulkanManager::storageBufferInfo[8];
std::vector<std::string> MyVulkanManager::storageBufferNames = {//存储缓冲名称的数组
        "dxPingpong1", "dyPingpong1","dzPingpong1",//X、Y、Z 轴方向的乒乓存储缓冲 1
        "dxPingpong2", "dyPingpong2","dzPingpong2",//X、Y、Z 轴方向的乒乓存储缓冲 2
        "h0kh0minusk","twiddleFactors"//保存了 h0(k)与 h0(-k)信息和扰动因子的存储缓冲
};
ShaderQueueSuit_SingleTexLight* MyVulkanManager::sqsSTL;
ShaderQueueSuit_ComputeButterfly*  MyVulkanManager::sqsButterfly;
ShaderQueueSuit_ComputeH0k*  MyVulkanManager::sqsH0k;
ShaderQueueSuit_ComputeHkt*  MyVulkanManager::sqsHkt;
ShaderQueueSuit_ComputeInversion*  MyVulkanManager::sqsInversion;
ShaderQueueSuit_ComputeTwiddleFactors*  MyVulkanManager::sqsTwiddleFactors;
ShaderQueueSuit_ComputeNormal*  MyVulkanManager::sqsNormal;
TexLightObject*  MyVulkanManager::waterForDraw;
float MyVulkanManager::choppiness = 50.0;
float MyVulkanManager::heightScale = 50;
float MyVulkanManager::windX = 1;
float MyVulkanManager::windY = 1;
float MyVulkanManager::windSpeed = 1200;
int MyVulkanManager::fftResolution = 256;
float MyVulkanManager::fftAmplitude = 1.8;
int MyVulkanManager::fftL = 1000;
float MyVulkanManager::fftCapillarwavesSuppression = 0.5;
float MyVulkanManager::t_delta = 0.05;
int MyVulkanManager::log_2_n = (int)(log(MyVulkanManager::fftResolution) / log(2));
int MyVulkanManager::stages = log_2_n;
float MyVulkanManager::time = 0;
int* MyVulkanManager::horizontalPushConstants;
int* MyVulkanManager::verticalPushConstants;
int* MyVulkanManager::inversionPushConstants;
void MyVulkanManager::init_vulkan_instance()
{
    AAssetManager* aam=MyVulkanManager::Android_application->activity->assetManager;
    FileUtil::setAAssetManager(aam);
    if (!vk::loadVulkan())
    {
        LOGI("加载Vulkan API失败!");
        return ;
    }
    instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instanceExtensionNames.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "HelloVulkan";
    app_info.applicationVersion = 1;
    app_info.pEngineName = "HelloVulkan";
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledExtensionCount = instanceExtensionNames.size();
    inst_info.ppEnabledExtensionNames = instanceExtensionNames.data();;
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = NULL;
    VkResult result;
    result = vk::vkCreateInstance(&inst_info, NULL, &instance);
    if(result== VK_SUCCESS)
    {
        LOGE("Vulkan实例创建成功!");
    }
    else
    {
        LOGE("Vulkan实例创建失败!");
    }
}
void MyVulkanManager::destroy_vulkan_instance()
{
    vk::vkDestroyInstance(instance, NULL);
    LOGE("Vulkan实例�?毁完�?!");
}
void MyVulkanManager::enumerate_vulkan_phy_devices()
{
    gpuCount=0;
    VkResult result = vk::vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
    assert(result==VK_SUCCESS);
    LOGE("[Vulkan硬件设备数量�?%d个]",gpuCount);
    gpus.resize(gpuCount);
    result = vk::vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
    assert(result==VK_SUCCESS);
    vk::vkGetPhysicalDeviceMemoryProperties(gpus[0],&memoryroperties);
}
void MyVulkanManager::create_vulkan_devices()
{
    vk::vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, NULL);
    LOGE("[Vulkan硬件设备0支持的队列家族数量为%d]",queueFamilyCount);
    queueFamilyprops.resize(queueFamilyCount);
    vk::vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, queueFamilyprops.data());
    LOGE("[成功获取Vulkan硬件设备0支持的队列家族属性列表]");
    VkDeviceQueueCreateInfo queueInfo = {};
    bool found = false;
    for (unsigned int i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueInfo.queueFamilyIndex = i;
            queueGraphicsFamilyIndex=i;
            LOGE("[支持GRAPHICS工作的一个队列家族的索引�?%d]",i);
            LOGE("[此家族中的实际队列数量是%d]",queueFamilyprops[i].queueCount);
            found = true;
            break;
        }
    }
    float queue_priorities[1] = {0.0};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queue_priorities;
    queueInfo.queueFamilyIndex = queueGraphicsFamilyIndex;
    deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = deviceExtensionNames.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = NULL;
    deviceInfo.pEnabledFeatures = NULL;
    VkResult result = vk::vkCreateDevice(gpus[0], &deviceInfo, NULL, &device);
    assert(result==VK_SUCCESS);
}
void MyVulkanManager::destroy_vulkan_devices()
{
    vk::vkDestroyDevice(device, NULL);
    LOGE("逻辑设备�?毁完毕！");
}
void MyVulkanManager::create_vulkan_CommandBuffer()
{
    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = queueGraphicsFamilyIndex;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkResult result = vk::vkCreateCommandPool(device, &cmd_pool_info, NULL, &cmdPool);
    assert(result==VK_SUCCESS);
    VkCommandBufferAllocateInfo cmdBAI = {};
    cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBAI.pNext = NULL;
    cmdBAI.commandPool = cmdPool;
    cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBAI.commandBufferCount = 1;
    result = vk::vkAllocateCommandBuffers(device, &cmdBAI, &cmdBuffer);
    assert(result==VK_SUCCESS);
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = NULL;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = NULL;
    cmd_bufs[0] = cmdBuffer;
    const VkPipelineStageFlags pipe_stage_flagsRender = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].pWaitDstStageMask = &pipe_stage_flagsRender;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;
    const VkPipelineStageFlags pipe_stage_flagsCompute = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    submit_info[1].pNext = NULL;
    submit_info[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[1].pWaitDstStageMask = &pipe_stage_flagsCompute;
    submit_info[1].commandBufferCount = 1;
    submit_info[1].pCommandBuffers = cmd_bufs;
    submit_info[1].signalSemaphoreCount = 0;
    submit_info[1].pSignalSemaphores = NULL;
    submit_info[1].waitSemaphoreCount = 0;
    submit_info[1].pWaitSemaphores = NULL;
}
void MyVulkanManager::destroy_vulkan_CommandBuffer()
{
    VkCommandBuffer cmdBufferArray[1] = {cmdBuffer};
    vk::vkFreeCommandBuffers
    (
         device, 
         cmdPool,
         1,      
         cmdBufferArray
    );
    vk::vkDestroyCommandPool(device, cmdPool, NULL);
}
void MyVulkanManager::create_vulkan_swapChain()
{
    screenWidth = ANativeWindow_getWidth(Android_application->window);
    screenHeight = ANativeWindow_getHeight(Android_application->window);
    LOGE("窗体宽度%d 窗体高度%d",screenWidth,screenHeight);
    VkAndroidSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.window = Android_application->window;
    PFN_vkCreateAndroidSurfaceKHR fpCreateAndroidSurfaceKHR=(PFN_vkCreateAndroidSurfaceKHR)vk::vkGetInstanceProcAddr(instance, "vkCreateAndroidSurfaceKHR");
    if (fpCreateAndroidSurfaceKHR == NULL)
    {
            LOGE( "找不到vkvkCreateAndroidSurfaceKHR扩展函数�?" );
    }
    VkResult result = fpCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface);
    assert(result==VK_SUCCESS);
    VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        vk::vkGetPhysicalDeviceSurfaceSupportKHR(gpus[0], i, surface, &pSupportsPresent[i]);
        LOGE("队列家族索引=%d %s显示",i,(pSupportsPresent[i]==1?"支持":"不支�?"));
    }
    queueGraphicsFamilyIndex = UINT32_MAX;
    queuePresentFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i <queueFamilyCount; ++i)
    {
        if ((queueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (queueGraphicsFamilyIndex== UINT32_MAX) queueGraphicsFamilyIndex = i;
            if (pSupportsPresent[i] == VK_TRUE)
            {
                queueGraphicsFamilyIndex = i;
                queuePresentFamilyIndex = i;
                LOGE("队列家族索引=%d 同时支持Graphis（图形）和Present（显示）工作",i);
                break;
            }
        }
    }
    if (queuePresentFamilyIndex == UINT32_MAX)
    {
        for (size_t i = 0; i < queueFamilyCount; ++i)
        {
            if (pSupportsPresent[i] == VK_TRUE)
            {
                queuePresentFamilyIndex= i;
                break;
            }
        }
    }
    free(pSupportsPresent);
    if (queueGraphicsFamilyIndex == UINT32_MAX || queuePresentFamilyIndex == UINT32_MAX)
    {
        LOGE("没有找到支持Graphis（图形）或Present（显示）工作的队列家�?");
        assert(false);
    }
    uint32_t formatCount;
    result = vk::vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, NULL);
    LOGE("支持的格式数量为 %d",formatCount);
    VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    formats.resize(formatCount);
    result = vk::vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, surfFormats);
    for(int i=0;i<formatCount;i++)
    {
        formats[i]=surfFormats[i].format;
        LOGE("[%d]支持的格式为%d",i,formats[i]);
    }
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        formats[0] = VK_FORMAT_B8G8R8A8_UNORM;
    }
    free(surfFormats);
    result = vk::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[0], surface, &surfCapabilities);
    assert(result == VK_SUCCESS);
    result = vk::vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, NULL);
    assert(result == VK_SUCCESS);
    LOGE("显示模式数量�?%d",presentModeCount);
    presentModes.resize(presentModeCount);
    result = vk::vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, presentModes.data());
    for(int i=0;i<presentModeCount;i++)
    {
        LOGE("显示模式[%d]编号�?%d",i,presentModes[i]);
    }
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR)&&(presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        swapchainExtent.width = screenWidth;
        swapchainExtent.height = screenHeight;
        if (swapchainExtent.width < surfCapabilities.minImageExtent.width)
        {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        }
        else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width)
        {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }
        if (swapchainExtent.height < surfCapabilities.minImageExtent.height)
        {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        } else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height)
        {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
        LOGE("使用自己设置�? 宽度 %d 高度 %d",swapchainExtent.width,swapchainExtent.height);
    }
    else
    {
        swapchainExtent = surfCapabilities.currentExtent;
        LOGE("使用获取的surface能力中的 宽度 %d 高度 %d",swapchainExtent.width,swapchainExtent.height);
    }
    screenWidth=swapchainExtent.width;
    screenHeight=swapchainExtent.height;
    uint32_t desiredMinNumberOfSwapChainImages = surfCapabilities.minImageCount+1;
    if ((surfCapabilities.maxImageCount > 0) &&(desiredMinNumberOfSwapChainImages > surfCapabilities.maxImageCount))
    {
        desiredMinNumberOfSwapChainImages = surfCapabilities.maxImageCount;
    }
    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfCapabilities.currentTransform;
    }
    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.pNext = NULL;
    swapchain_ci.surface = surface;
    swapchain_ci.minImageCount = desiredMinNumberOfSwapChainImages;
    swapchain_ci.imageFormat = formats[0];
    swapchain_ci.imageExtent.width = swapchainExtent.width;
    swapchain_ci.imageExtent.height = swapchainExtent.height;
    swapchain_ci.preTransform = preTransform;
    swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swapchainPresentMode;
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
    swapchain_ci.clipped = true;
    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = NULL;
    if (queueGraphicsFamilyIndex != queuePresentFamilyIndex)
    {
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;
        uint32_t queueFamilyIndices[2] = {queueGraphicsFamilyIndex,queuePresentFamilyIndex};
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
    }
    result = vk::vkCreateSwapchainKHR(device, &swapchain_ci, NULL, &swapChain);
    assert(result == VK_SUCCESS);
    result = vk::vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, NULL);
    assert(result == VK_SUCCESS);
    LOGE("[SwapChain中的Image数量�?%d]",swapchainImageCount);
    swapchainImages.resize(swapchainImageCount);
    result = vk::vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, swapchainImages.data());
    assert(result == VK_SUCCESS);
    swapchainImageViews.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.flags = 0;
        color_image_view.image = swapchainImages[i];
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.format = formats[0];
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;
        result = vk::vkCreateImageView(device, &color_image_view, NULL, &swapchainImageViews[i]);
        assert(result == VK_SUCCESS);
    }
}
void MyVulkanManager::destroy_vulkan_swapChain()
{
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        vk::vkDestroyImageView(device, swapchainImageViews[i], NULL);
        LOGE("[�?毁SwapChain ImageView %d 成功]",i);
    }
    vk::vkDestroySwapchainKHR(device, swapChain, NULL);
    LOGE("�?毁SwapChain成功!");
}
void MyVulkanManager::create_vulkan_DepthBuffer()
{
    depthFormat = VK_FORMAT_D16_UNORM;
    VkImageCreateInfo image_info = {};
    vk::vkGetPhysicalDeviceFormatProperties(gpus[0], depthFormat, &depthFormatProps);
    if (depthFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
        LOGE("tiling为VK_IMAGE_TILING_LINEAR�?");
    }
    else if (depthFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        LOGE("tiling为VK_IMAGE_TILING_OPTIMAL�?");
    }
    else
    {
        LOGE("不支持VK_FORMAT_D16_UNORM�?");
    }
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = NULL;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = depthFormat;
    image_info.extent.width = screenWidth;
    image_info.extent.height =screenHeight;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = NULL;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;
    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = NULL;
    view_info.image = VK_NULL_HANDLE;
    view_info.format = depthFormat;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;
    VkResult result = vk::vkCreateImage(device, &image_info, NULL, &depthImage);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vk::vkGetImageMemoryRequirements(device, depthImage, &mem_reqs);
    mem_alloc.allocationSize = mem_reqs.size;
    VkFlags requirements_mask=0;
    bool flag=memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits,requirements_mask,&mem_alloc.memoryTypeIndex);
    assert(flag);
    LOGE("确定内存类型成功 类型索引�?%d",mem_alloc.memoryTypeIndex);
    result = vk::vkAllocateMemory(device, &mem_alloc, NULL, &memDepth);
    assert(result == VK_SUCCESS);
    result = vk::vkBindImageMemory(device, depthImage, memDepth, 0);
    assert(result == VK_SUCCESS);
    view_info.image = depthImage;
    result = vk::vkCreateImageView(device, &view_info, NULL, &depthImageView);
    assert(result == VK_SUCCESS);
}
void MyVulkanManager::destroy_vulkan_DepthBuffer()
{
    vk::vkDestroyImageView(device, depthImageView, NULL);
    vk::vkDestroyImage(device, depthImage, NULL);
    vk::vkFreeMemory(device, memDepth, NULL);
    LOGE("�?毁深度缓冲相关成�?!");
}
void MyVulkanManager::create_render_pass()
{
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = NULL;
    imageAcquiredSemaphoreCreateInfo.flags = 0;
    VkResult result = vk::vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
    assert(result == VK_SUCCESS);
    VkAttachmentDescription attachments[2];
    attachments[0].format = formats[0];
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].flags = 0;
    attachments[1].format = depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].flags = 0;
    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = NULL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = &depth_reference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;
    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = NULL;
    rp_info.attachmentCount = 2;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = NULL;
    result = vk::vkCreateRenderPass(device, &rp_info, NULL, &renderPass);
    assert(result == VK_SUCCESS);
    clear_values[0].color.float32[0] = 0.0f;
    clear_values[0].color.float32[1] = 0.0f;
    clear_values[0].color.float32[2] = 0.0f;
    clear_values[0].color.float32[3] = 0.0f;
    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = NULL;
    rp_begin.renderPass = renderPass;
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = screenWidth;
    rp_begin.renderArea.extent.height = screenHeight;
    rp_begin.clearValueCount = 2;
    rp_begin.pClearValues = clear_values;
}
void MyVulkanManager::destroy_render_pass()
{
    vk::vkDestroyRenderPass(device, renderPass, NULL);
    vk::vkDestroySemaphore(device, imageAcquiredSemaphore, NULL);
}
void MyVulkanManager::init_queue()
{
    vk::vkGetDeviceQueue(device, queueGraphicsFamilyIndex, 0,&queueGraphics);
}
void MyVulkanManager::create_frame_buffer()
{
    VkImageView attachments[2];
    attachments[1] = depthImageView;
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = NULL;
    fb_info.renderPass = renderPass;
    fb_info.attachmentCount = 2;
    fb_info.pAttachments = attachments;
    fb_info.width = screenWidth;
    fb_info.height = screenHeight;
    fb_info.layers = 1;
    uint32_t i;
    framebuffers = (VkFramebuffer *)malloc(swapchainImageCount * sizeof(VkFramebuffer));
    assert(framebuffers);
    for (i = 0; i < swapchainImageCount; i++)
    {
        attachments[0] = swapchainImageViews[i];
        VkResult result = vk::vkCreateFramebuffer(device, &fb_info, NULL, &framebuffers[i]);
        assert(result == VK_SUCCESS);
        LOGE("[创建帧缓�?%d成功！]",i);
    }
}
void MyVulkanManager::destroy_frame_buffer()
{
    for (int i = 0; i < swapchainImageCount; i++)
    {
        vk::vkDestroyFramebuffer(device, framebuffers[i], NULL);
    }
    free(framebuffers);
    LOGE("�?毁帧缓冲成功�?");
}
void MyVulkanManager::createDrawableObject()
{
    VertData::genVertData();
    waterForDraw=new TexLightObject(VertData::vdata,VertData::dataByteCount,VertData::vCount,VertData::indexData,VertData::indexDataByteCount,VertData::indexCount,device, memoryroperties);
}
void MyVulkanManager::destroyDrawableObject()
{
    delete waterForDraw;
}
void MyVulkanManager::createFence()
{
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vk::vkCreateFence(device, &fenceInfo, NULL, &taskFinishFence);
}
void MyVulkanManager::destroyFence()
{
    vk::vkDestroyFence(device, taskFinishFence, NULL);
}
void MyVulkanManager::initPresentInfo()
{
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain;
    present.pWaitSemaphores = NULL;
    present.waitSemaphoreCount = 0;
    present.pResults = NULL;
}
void MyVulkanManager::initMatrixAndLight()
{
    CameraUtil::calCamera(0,0);
    MatrixState3D::setInitStack();
    float ratio=(float)screenWidth/(float)screenHeight;
    MatrixState3D::setProjectFrustum(-ratio,ratio,-1,1,4.0f,5000);
    LightManager::setLightPosition(1305, 100, 0);
    LightManager::setlightAmbient(0.1f,0.1f,0.1f,0.1f);
    LightManager::setlightDiffuse(0.9f,0.9f,0.5f,0.8f);
    LightManager::setlightSpecular(0.4f,0.4f,0.25f,0.4f);
}
int MyVulkanManager::getStorageBufferIndex(std::string texName)
{//根据存储缓冲名称查询其索引的方法
    int result = -1;//存储结果索引值的变量
    for (int i = 0; i<storageBufferNames.size(); i++)//遍历储缓冲的名称数组
    {
        if (storageBufferNames[i].compare(texName.c_str()) == 0)//与存储缓冲名称数组中的名称对比
        {
            result = i;//找到目标索引值
            break;
        }
    }
    assert(result != -1);//没有找到则触发断言
    return result;//返回结果
}
void MyVulkanManager::flushUniformBufferForDraw()
{
    float vertexUniformData[20]=
    {
            CameraUtil::camera9Para[0],CameraUtil::camera9Para[1],CameraUtil::camera9Para[2],1.0,                                   
            LightManager::lx,LightManager::ly,LightManager::lz,1.0,                                                                 
            LightManager::lightAmbientR,LightManager::lightAmbientG,LightManager::lightAmbientB,LightManager::lightAmbientA,        
            LightManager::lightDiffuseR,LightManager::lightDiffuseG,LightManager::lightDiffuseB,LightManager::lightDiffuseA,        
            LightManager::lightSpecularR,LightManager::lightSpecularG,LightManager::lightSpecularB,LightManager::lightSpecularA     
    };
    uint8_t *pData;
    VkResult result = vk::vkMapMemory(device, sqsSTL->memUniformBuf, 0, sqsSTL->bufferByteCount, 0, (void **)&pData);
    assert(result==VK_SUCCESS);
    memcpy(pData, vertexUniformData, sqsSTL->bufferByteCount);
    vk::vkUnmapMemory(device,sqsSTL->memUniformBuf);
}
void MyVulkanManager::flushTexToDesSetForDraw()
{
    sqsSTL->writes[0].dstSet = sqsSTL->descSet[0];
    sqsSTL->writes[1].dstSet = sqsSTL->descSet[0];
    sqsSTL->writes[1].pImageInfo = &(TextureManager::texImageInfoList[TextureManager::texNames[0]]);
    vk::vkUpdateDescriptorSets(device, 2, sqsSTL->writes, 0, NULL);
}
void MyVulkanManager::flushTexToDesSetForNormal(){ 
    sqsNormal->writes[0].dstSet = sqsNormal->descSet[0]; 
    sqsNormal->writes[0].pBufferInfo=&(waterForDraw->vertexDataBufferInfoCompute);
    vk::vkUpdateDescriptorSets(device, 1, sqsNormal->writes, 0, NULL);
}
void MyVulkanManager::drawTask()
{
    CameraUtil::flushCameraToMatrix();
    VkResult result = vk::vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE,&currentBuffer);
    rp_begin.framebuffer = framebuffers[currentBuffer];
    vk::vkResetCommandBuffer(cmdBuffer, 0);
    result = vk::vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    MyVulkanManager::flushUniformBufferForDraw();
    MyVulkanManager::flushTexToDesSetForDraw();
    vk::vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    MatrixState3D::pushMatrix();
    waterForDraw->drawSelf(cmdBuffer,sqsSTL->pipelineLayout,sqsSTL->pipeline,
                          &(sqsSTL->descSet[TextureManager::getVkDescriptorSetIndexForCommonTexLight("texture/haimian.bntex")]));
    MatrixState3D::popMatrix();
    vk::vkCmdEndRenderPass(cmdBuffer);
    result = vk::vkEndCommandBuffer(cmdBuffer);
    submit_info[0].waitSemaphoreCount = 1;
    submit_info[0].pWaitSemaphores =  &imageAcquiredSemaphore;
    result = vk::vkQueueSubmit(queueGraphics, 1, &submit_info[0], taskFinishFence);
    do
    {
        result = vk::vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    }
    while (result == VK_TIMEOUT);
    vk::vkResetFences(device,1,&taskFinishFence);
    present.pImageIndices = &currentBuffer;
    result = vk::vkQueuePresentKHR(queueGraphics, &present);
}
void MyVulkanManager::calTaskTwiddleFactors(){//计算扰动因子的方法
    vk::vkResetCommandBuffer(cmdBuffer, 0);
    VkResult result = vk::vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    sqsTwiddleFactors->writes[0].dstSet = sqsTwiddleFactors->descSet[0];//更新描述集对应的写入属性 0
    sqsTwiddleFactors->writes[0].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("twiddleFactors")]);//绑定扰动因子的存储缓冲
    sqsTwiddleFactors->writes[1].dstSet = sqsTwiddleFactors->descSet[0]; //更新描述集对应的写入属性 1
    sqsTwiddleFactors->writes[1].pBufferInfo = &(sqsTwiddleFactors->bit_reversedBufferInfoCompute);//绑定辅助数组的存储缓冲
    vk::vkUpdateDescriptorSets(device, 2, sqsTwiddleFactors->writes, 0, NULL);//更新描述集
    vk::vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, sqsTwiddleFactors->pipeline);//绑定到计算管线
    vk::vkCmdBindDescriptorSets(cmdBuffer, //将命令缓冲、管线布局、描述集绑定
                            VK_PIPELINE_BIND_POINT_COMPUTE, sqsTwiddleFactors->pipelineLayout, 0, 1, &sqsTwiddleFactors->descSet[0], 0, NULL);
    int pushConstantData[1] = {//推送常量数据
            fftResolution
    };
    vk::vkCmdPushConstants(cmdBuffer, sqsTwiddleFactors->pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(int) * 1, pushConstantData);//将推送常量数据送入计算管线
    vk::vkCmdDispatch(cmdBuffer, log_2_n, fftResolution, 1); //将计算任务派送到计算管线
    result = vk::vkEndCommandBuffer(cmdBuffer);
    result = vk::vkQueueSubmit(queueGraphics, 1, &submit_info[1], taskFinishFence);
    do{
        result = vk::vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vk::vkResetFences(device, 1, &taskFinishFence);
}
void MyVulkanManager::calTaskH0kH0minusk(){//计算 h0(k)和 h0(-k)的方法
    vk::vkResetCommandBuffer(cmdBuffer, 0);
    VkResult result = vk::vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    sqsH0k->writes[0].dstSet = sqsH0k->descSet[0];//更新描述集对应的写入属性 0
    sqsH0k->writes[0].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("h0kh0minusk")]);//绑定 h0(k)和 h0(-k)的存储缓冲
    sqsH0k->writes[1].dstSet = sqsH0k->descSet[0];//更新描述集对应的写入属性 1
    sqsH0k->writes[1].pImageInfo = &(TextureManager::texImageInfoList["texture/Noise256_0.bntex"]);//绑定噪声纹理缓冲 0
    sqsH0k->writes[2].dstSet = sqsH0k->descSet[0];//更新描述集对应的写入属性 2
    sqsH0k->writes[2].pImageInfo = &(TextureManager::texImageInfoList["texture/Noise256_1.bntex"]);//绑定噪声纹理缓冲 1
    sqsH0k->writes[3].dstSet = sqsH0k->descSet[0];//更新描述集对应的写入属性 3
    sqsH0k->writes[3].pImageInfo = &(TextureManager::texImageInfoList["texture/Noise256_2.bntex"]);//绑定噪声纹理缓冲 2
    sqsH0k->writes[4].dstSet = sqsH0k->descSet[0];//更新描述集对应的写入属性 4
    sqsH0k->writes[4].pImageInfo = &(TextureManager::texImageInfoList["texture/Noise256_3.bntex"]);//绑定噪声纹理缓冲 3
    vk::vkUpdateDescriptorSets(device, 5, sqsH0k->writes, 0, NULL);//更新描述集
    vk::vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, sqsH0k->pipeline); //绑定到计算管线
    vk::vkCmdBindDescriptorSets(cmdBuffer, //将命令缓冲、管线布局、描述集绑定
                            VK_PIPELINE_BIND_POINT_COMPUTE, sqsH0k->pipelineLayout, 0, 1, &sqsH0k->descSet[0], 0, NULL);
    float pushConstantData[7] = {//组织推送常量数据
            (float)fftResolution,(float)fftL,fftAmplitude,windSpeed,windX,windY,fftCapillarwavesSuppression
    };
    vk::vkCmdPushConstants(cmdBuffer, sqsH0k->pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float) * 7, pushConstantData); //将推送常量数据送入计算管线
    vk::vkCmdDispatch(cmdBuffer,fftResolution, fftResolution, 1);  //将计算任务派送到计算管线
    result = vk::vkEndCommandBuffer(cmdBuffer);
    result = vk::vkQueueSubmit(queueGraphics, 1, &submit_info[1], taskFinishFence);
    do{
        result = vk::vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vk::vkResetFences(device, 1, &taskFinishFence);
}
void MyVulkanManager::calTaskButterfly(std::string pingpong0, std::string pingpong1,int stage, int pingpong,int direction){//执行一次一维蝴蝶计算的方法
    vk::vkResetCommandBuffer(cmdBuffer, 0);
    VkResult result = vk::vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    int vertexUniformData[1] = { log_2_n };//一致变量数组
    uint8_t *pData;//CPU 访问设备内存时的辅助指针
    result = vk::vkMapMemory(device, sqsButterfly->memUniformBuf, 0, sqsButterfly->bufferByteCount, 0, (void **)&pData);//将设备内存映射为 CPU 可访问
    assert(result == VK_SUCCESS);//检查映射是否成功
    memcpy(pData, vertexUniformData, sqsButterfly->bufferByteCount);
    vk::vkUnmapMemory(device, sqsButterfly->memUniformBuf);
    sqsButterfly->writes[0].dstSet = sqsButterfly->descSet[0];//更新描述集对应的写入属性 0
    sqsButterfly->writes[0].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("twiddleFactors")]);//绑定扰动因子的存储缓冲
    sqsButterfly->writes[1].dstSet = sqsButterfly->descSet[0];//更新描述集对应的写入属性 1
    sqsButterfly->writes[1].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex(pingpong0)]);//绑定对应的乒乓存储缓冲 0
    sqsButterfly->writes[2].dstSet = sqsButterfly->descSet[0];//更新描述集对应的写入属性 2
    sqsButterfly->writes[2].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex(pingpong1)]);//绑定对应的乒乓存储缓冲 1
    sqsButterfly->writes[3].dstSet = sqsButterfly->descSet[0];//更新描述集对应的写入属性 1
    sqsButterfly->writes[3].pBufferInfo = &(sqsButterfly->uniformBufferInfo); //绑定计算用的一致变量缓冲
    vk::vkUpdateDescriptorSets(device, 4, sqsButterfly->writes, 0, NULL);//更新描述集
    vk::vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, sqsButterfly->pipeline);//绑定到计算管线
    vk::vkCmdBindDescriptorSets(cmdBuffer, //将命令缓冲、管线布局、描述集绑定
                            VK_PIPELINE_BIND_POINT_COMPUTE, sqsButterfly->pipelineLayout, 0, 1, &sqsButterfly->descSet[0], 0, NULL);
    int pushConstantData[3] = {
            stage, pingpong,direction//组织推送常量数据
    };
    vk::vkCmdPushConstants(cmdBuffer, sqsButterfly->pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(int) * 3, pushConstantData); //将推送常量数据送入计算管线
    vk::vkCmdDispatch(cmdBuffer, fftResolution, fftResolution, 1); //将计算任务派送到计算管线
    result = vk::vkEndCommandBuffer(cmdBuffer);
    result = vk::vkQueueSubmit(queueGraphics, 1, &submit_info[1], taskFinishFence);
    do {
        result = vk::vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vk::vkResetFences(device, 1, &taskFinishFence);
}
void MyVulkanManager::calTaskInversion(int N, int pingpong) {//将计算结果转换为绘制用数据的方法
    vk::vkResetCommandBuffer(cmdBuffer, 0);
    VkResult result = vk::vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    sqsInversion->writes[0].dstSet = sqsInversion->descSet[0];//更新描述集对应的写入属性 0
    sqsInversion->writes[1].dstSet = sqsInversion->descSet[0];//更新描述集对应的写入属性 1
    sqsInversion->writes[2].dstSet = sqsInversion->descSet[0];//更新描述集对应的写入属性 2
    if (pingpong == 0) {//若 FFT 结果存储在 Pingpong2 中
        sqsInversion->writes[0].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dyPingpong2")]);//绑定 Y 轴的乒乓存储缓冲 2
        sqsInversion->writes[1].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dxPingpong2")]);//绑定 X 轴的乒乓存储缓冲 2
        sqsInversion->writes[2].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dzPingpong2")]);//绑定 Z 轴的乒乓存储缓冲 2
    }
    else {//若 FFT 结果存储在 Pingpong1 中
        sqsInversion->writes[0].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dyPingpong1")]);//绑定 Y 轴的乒乓存储缓冲 1
        sqsInversion->writes[1].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dxPingpong1")]);//绑定 X 轴的乒乓存储缓冲 1
        sqsInversion->writes[2].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dzPingpong1")]);//绑定 Z 轴的乒乓存储缓冲 1
    }
    sqsInversion->writes[3].dstSet = sqsInversion->descSet[0];//更新描述集对应的写入属性 3
    sqsInversion->writes[3].pBufferInfo = &(waterForDraw->vertexDataBufferInfo);//绑定计算用的存储缓冲
    sqsInversion->writes[4].dstSet = sqsInversion->descSet[0]; //更新描述集对应的写入属性 4
    sqsInversion->writes[4].pBufferInfo = &(waterForDraw->vertexDataBufferInfoCompute);//绑定计算用的存储缓冲
    vk::vkUpdateDescriptorSets(device, 5, sqsInversion->writes, 0, NULL);//更新描述集
    vk::vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, sqsInversion->pipeline);//绑定到计算管线
    vk::vkCmdBindDescriptorSets(cmdBuffer, //将命令缓冲、管线布局、描述集绑定
                            VK_PIPELINE_BIND_POINT_COMPUTE, sqsInversion->pipelineLayout, 0, 1, &sqsInversion->descSet[0], 0, NULL);
    float pushConstantData[3] = {
            (float)N,heightScale,choppiness//组织推送常量数据
    };
    vk::vkCmdPushConstants(cmdBuffer, sqsInversion->pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float) * 3, pushConstantData); //将推送常量数据送入计算管线
    vk::vkCmdDispatch(cmdBuffer, fftResolution, fftResolution, 1); //将计算任务派送到计算管线
    result = vk::vkEndCommandBuffer(cmdBuffer);
    result = vk::vkQueueSubmit(queueGraphics, 1, &submit_info[1], taskFinishFence);
    do {
        result = vk::vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vk::vkResetFences(device, 1, &taskFinishFence);
}
void MyVulkanManager::calTaskHkt(){//计算傅里叶级数振幅分量的方法
    vk::vkResetCommandBuffer(cmdBuffer, 0);
    VkResult result = vk::vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    time += t_delta;//时间推进
    float vertexUniformData[1] ={time};//一致变量数组
    uint8_t *pData;//CPU 访问设备内存时的辅助指针
    result = vk::vkMapMemory(device, sqsHkt->memUniformBuf, 0, sqsHkt->bufferByteCount, 0, (void **)&pData); //将设备内存映射为 CPU 可访问
    assert(result == VK_SUCCESS);//检查映射是否成功
    memcpy(pData, vertexUniformData, sqsHkt->bufferByteCount);//将数据拷贝进设备内存
    vk::vkUnmapMemory(device, sqsHkt->memUniformBuf);//解除内存映射
    sqsHkt->writes[0].dstSet = sqsHkt->descSet[0];//更新描述集对应的写入属性 0
    sqsHkt->writes[0].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dyPingpong2")]);//绑定 Y 轴的乒乓存储缓冲 2
    sqsHkt->writes[1].dstSet = sqsHkt->descSet[0];//更新描述集对应的写入属性 1
    sqsHkt->writes[1].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dxPingpong2")]);//绑定 X 轴的乒乓存储缓冲 2
    sqsHkt->writes[2].dstSet = sqsHkt->descSet[0];//更新描述集对应的写入属性 2
    sqsHkt->writes[2].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("dzPingpong2")]);//绑定 Z 轴的乒乓存储缓冲 2
    sqsHkt->writes[3].dstSet = sqsHkt->descSet[0];//更新描述集对应的写入属性 3
    sqsHkt->writes[3].pBufferInfo = &(storageBufferInfo[getStorageBufferIndex("h0kh0minusk")]);//绑定 h0(k)和 h0(-k)的存储缓冲
    sqsHkt->writes[4].dstSet = sqsHkt->descSet[0];//更新描述集对应的写入属性 4
    sqsHkt->writes[4].pBufferInfo = &(sqsHkt->uniformBufferInfo); //绑定 h(k,t)的存储缓冲
    vk::vkUpdateDescriptorSets(device, 5, sqsHkt->writes, 0, NULL); //更新描述集
    vk::vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, sqsHkt->pipeline);//绑定到计算管线
    vk::vkCmdBindDescriptorSets(cmdBuffer, //将命令缓冲、管线布局、描述集绑定
                            VK_PIPELINE_BIND_POINT_COMPUTE, sqsHkt->pipelineLayout, 0, 1, &sqsHkt->descSet[0], 0, NULL);
    int pushConstantData[2] = {//组织推送常量数据
            fftResolution,fftL
    };
    vk::vkCmdPushConstants(cmdBuffer, sqsHkt->pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(int) * 2, pushConstantData);//将推送常量数据送入计算管线
    vk::vkCmdDispatch(cmdBuffer, fftResolution, fftResolution, 1); //将计算任务派送到计算管线
    result = vk::vkEndCommandBuffer(cmdBuffer);
    result = vk::vkQueueSubmit(queueGraphics, 1, &submit_info[1], taskFinishFence);
    do {
        result = vk::vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vk::vkResetFences(device, 1, &taskFinishFence);
}
void MyVulkanManager::calTaskNormal(){
    vk::vkResetCommandBuffer(cmdBuffer, 0);
    VkResult result = vk::vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
    MyVulkanManager::flushTexToDesSetForNormal();
    waterForDraw->calSelfNormal(cmdBuffer, sqsNormal->pipelineLayout,
                                sqsNormal->pipeline, &(sqsNormal->descSet[0]));
    result = vk::vkEndCommandBuffer(cmdBuffer);
    result = vk::vkQueueSubmit(queueGraphics, 1, &submit_info[1], taskFinishFence);
    do{
        result = vk::vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    vk::vkResetFences(device, 1, &taskFinishFence);
}
void MyVulkanManager::calOceanFFTPara() {//计算 FFT 辅助常量的方法
    horizontalPushConstants = (int*) malloc(sizeof(int)*stages*3);//申请空间
    verticalPushConstants= (int*)malloc(sizeof(int)*stages*3);//申请空间
    int pingpong = 0;//初始化时乒乓存储缓冲 1 为输入
    for (int i = 0; i<stages; i++) {
        horizontalPushConstants[i*3] = i;//记录当前阶段
        horizontalPushConstants[i*3+1] = pingpong;//记录乒乓存储缓冲角色
        horizontalPushConstants[i*3+2] = 0;//记录方向为横向
        pingpong++;//交换乒乓存储缓冲角色
        pingpong %= 2;
    }
    for (int i = 0; i<stages; i++) {
        verticalPushConstants[i * 3] = i;//记录当前阶段
        verticalPushConstants[i * 3 + 1] = pingpong;//记录乒乓存储缓冲角色
        verticalPushConstants[i * 3 + 2] = 1;//记录方向为竖向
        pingpong++;//交换乒乓存储缓冲角色
        pingpong %= 2;
    }
    inversionPushConstants = new int[1];//申请空间
    inversionPushConstants[0] = pingpong;//记录乒乓存储缓冲角色
}
void MyVulkanManager::drawObject()
{//调用一次产生一帧画面的方法
    calOceanFFTPara();//初始化 FFT 所需各项常量的方法
    calTaskTwiddleFactors();//计算扰动因子的方法
    calTaskH0kH0minusk();//计算 h0(k)与 h0(-k)的方法
    FPSUtil::init();//初始化 FPS 工具类
    while (MyVulkanManager::loopDrawFlag)
    {//渲染循环
        FPSUtil::calFPS();//计算 FPS
        FPSUtil::before();//一帧的开始
        calTaskHkt();//执行计算 h(k,t)的方法
        for (int i = 0; i<stages; i++) {//执行横向蝴蝶计算
            calTaskButterfly("dyPingpong2", "dyPingpong1"//Y 轴方向
                    , horizontalPushConstants[i * 3], horizontalPushConstants[i * 3 + 1], horizontalPushConstants[i * 3 + 2]);
            calTaskButterfly("dxPingpong2", "dxPingpong1"//X 轴方向
                    , horizontalPushConstants[i * 3], horizontalPushConstants[i * 3 + 1], horizontalPushConstants[i * 3 + 2]);
            calTaskButterfly("dzPingpong2", "dzPingpong1"//Z 轴方向
                    , horizontalPushConstants[i * 3], horizontalPushConstants[i * 3 + 1], horizontalPushConstants[i * 3 + 2]);
        }
        for (int i = 0; i<stages; i++) {//执行竖向蝴蝶计算
            calTaskButterfly("dyPingpong2", "dyPingpong1"//Y 轴方向
                    , verticalPushConstants[i * 3], verticalPushConstants[i * 3 + 1], verticalPushConstants[i * 3 + 2]);
            calTaskButterfly("dxPingpong2", "dxPingpong1"//X 轴方向
                    , verticalPushConstants[i * 3], verticalPushConstants[i * 3 + 1], verticalPushConstants[i * 3 + 2]);
            calTaskButterfly("dzPingpong2", "dzPingpong1"//Z 轴方向
                    , verticalPushConstants[i * 3], verticalPushConstants[i * 3 + 1], verticalPushConstants[i * 3 + 2]);
        }
        calTaskInversion(fftResolution,inversionPushConstants[0]);//将计算结果转换为绘制用数据
        calTaskNormal(); //计算当前海面各个顶点的法向量
        drawTask(); //绘制海面
        FPSUtil::after(60);//限制 FPS 不超过指定的值
    }
}
void MyVulkanManager::doVulkan()
{
    ThreadTask* tt=new ThreadTask();
    thread t1(&ThreadTask::doTask,tt);
    t1.detach();
}
void MyVulkanManager::create_vulkan_StorageBuffer(int index) { //创建存储缓冲的方法
    int dataByteCount = fftResolution*fftResolution * 4 * sizeof(float);//计算存储缓冲的字节数
    if (index == getStorageBufferIndex("twiddleFactors"))//如果为扰动因子的存储缓冲
        dataByteCount = fftResolution*log_2_n * 4*sizeof(float);//重新计算存储缓冲的字节数
    VkBufferCreateInfo buf_info = {};//缓冲创建信息
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = NULL;
    buf_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;//设置缓冲用途
    buf_info.size = dataByteCount;//设置创建的存储缓冲的字节数
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = NULL;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    VkResult result = vk::vkCreateBuffer(device, &buf_info, NULL, &storageBuffer[index]);//创建缓冲
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vk::vkGetBufferMemoryRequirements(device, storageBuffer[index], &mem_reqs);
    assert(dataByteCount <= mem_reqs.size);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;//给定类型掩码
    bool flag = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
    if (flag)
    {
        printf("确定内存类型成功 类型索引�?%d\n", alloc_info.memoryTypeIndex);
    }
    else
    {
        printf("确定内存类型失败!\n");
    }
    result = vk::vkAllocateMemory(device, &alloc_info, NULL, &memStorageBuffer[index]);
    assert(result == VK_SUCCESS);
    result = vk::vkBindBufferMemory(device, storageBuffer[index], memStorageBuffer[index], 0);
    assert(result == VK_SUCCESS);
    storageBufferInfo[index].buffer = storageBuffer[index];//设置缓冲区信息的缓冲
    storageBufferInfo[index].offset = 0;//设置缓冲区信息的偏移量
    storageBufferInfo[index].range = mem_reqs.size;//设置缓冲区信息的字节数
}
void MyVulkanManager::destroy_vulkan_StorageBuffer(int index) { 
    vk::vkDestroyBuffer(device, storageBuffer[index], NULL);
    vk::vkFreeMemory(device, memStorageBuffer[index], NULL);
}
void MyVulkanManager::create_vulkan_StorageBuffers() {
    for (int i = 0; i < storageBufferNames.size(); i++) {
        create_vulkan_StorageBuffer(i);
    }
}
void MyVulkanManager::destroy_vulkan_StorageBuffers() {
    for (int i = 0; i < storageBufferNames.size(); i++) {
        destroy_vulkan_StorageBuffer(i);
    }
}
void MyVulkanManager::init_texture()
{
    TextureManager::initTextures(device,gpus[0],memoryroperties,cmdBuffer,queueGraphics);
}
void MyVulkanManager::destroy_textures()
{
    TextureManager::destroyTextures(device);
}
void MyVulkanManager::initPipeline()
{
    sqsSTL = new ShaderQueueSuit_SingleTexLight(&device, renderPass, memoryroperties);
    sqsButterfly = new ShaderQueueSuit_ComputeButterfly(&device, memoryroperties);
    sqsH0k = new ShaderQueueSuit_ComputeH0k(&device, memoryroperties);
    sqsHkt = new ShaderQueueSuit_ComputeHkt(&device, memoryroperties);
    sqsInversion = new ShaderQueueSuit_ComputeInversion(&device, memoryroperties);
    sqsTwiddleFactors = new ShaderQueueSuit_ComputeTwiddleFactors(fftResolution,&device, memoryroperties);
    sqsNormal = new ShaderQueueSuit_ComputeNormal(&device, memoryroperties);
}
void MyVulkanManager::destroyPipeline()
{
    delete sqsSTL;
    delete sqsButterfly;
    delete sqsH0k;
    delete sqsHkt;
    delete sqsInversion;
    delete sqsTwiddleFactors;
    delete sqsNormal;
}
