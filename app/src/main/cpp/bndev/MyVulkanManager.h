#ifndef VULKANEXBASE_MYVULKANMANAGER_H
#define VULKANEXBASE_MYVULKANMANAGER_H
#include <android_native_app_glue.h>
#include <vector>
#include <vulkan/vulkan.h>
#include "../vksysutil/vulkan_wrapper.h"
#include <vector>
#include "mylog.h"
#include "TexLightObject.h"
#include "ShaderQueueSuit_SingleTexLight.h"
#include "ShaderQueueSuit_ComputeNormal.h"
#include "ShaderQueueSuit_ComputeButterfly.h"
#include "ShaderQueueSuit_ComputeH0k.h"
#include "ShaderQueueSuit_ComputeHkt.h"
#include "ShaderQueueSuit_ComputeInversion.h"
#include "ShaderQueueSuit_ComputeTwiddleFactors.h"
#define FENCE_TIMEOUT 100000000
class MyVulkanManager
{
  public:
    static android_app* Android_application;
    static bool loopDrawFlag;
    static std::vector<const char *> instanceExtensionNames;
    static VkInstance instance;
    static uint32_t gpuCount;
    static  std::vector<VkPhysicalDevice> gpus;
    static uint32_t queueFamilyCount;
    static std::vector<VkQueueFamilyProperties> queueFamilyprops;
    static uint32_t queueGraphicsFamilyIndex;
    static VkQueue queueGraphics;
    static uint32_t queuePresentFamilyIndex;
    static std::vector<const char *> deviceExtensionNames;
    static VkDevice device;
    static VkCommandPool cmdPool;
    static VkCommandBuffer cmdBuffer;
    static VkCommandBufferBeginInfo cmd_buf_info;
    static VkCommandBuffer cmd_bufs[1];
    static VkSubmitInfo submit_info[2];
    static uint32_t screenWidth;
    static uint32_t screenHeight;
    static VkSurfaceKHR surface;
    static std::vector<VkFormat> formats;
    static VkSurfaceCapabilitiesKHR surfCapabilities;
    static uint32_t presentModeCount;
    static std::vector<VkPresentModeKHR> presentModes;
    static VkExtent2D swapchainExtent;
    static VkSwapchainKHR swapChain;
    static uint32_t swapchainImageCount;
    static std::vector<VkImage> swapchainImages;
    static std::vector<VkImageView> swapchainImageViews;
    static VkFormat depthFormat;
    static VkFormatProperties depthFormatProps;
    static VkImage depthImage;
    static VkPhysicalDeviceMemoryProperties memoryroperties;
    static VkDeviceMemory memDepth;
    static VkImageView depthImageView;
    static VkSemaphore imageAcquiredSemaphore;
    static uint32_t currentBuffer;
    static VkRenderPass renderPass;
    static VkClearValue clear_values[2];
    static VkRenderPassBeginInfo rp_begin;
    static VkFence taskFinishFence;
    static VkPresentInfoKHR present;
    static VkFramebuffer* framebuffers;
    static VkBuffer storageBuffer[8];//存储缓冲数组
    static VkDeviceMemory memStorageBuffer[8];//存储缓冲对应的设备内存数组
    static VkDescriptorBufferInfo storageBufferInfo[8];//存储缓冲对应的设备内存数组
    static std::vector<std::string> storageBufferNames;//存储缓冲的名称列表
    static int getStorageBufferIndex(std::string ss);//通过存储缓冲的名称查找索引的方法
    static ShaderQueueSuit_SingleTexLight* sqsSTL;//渲染着色器管线指针
    static ShaderQueueSuit_ComputeButterfly*  sqsButterfly;//蝴蝶计算着色器管线指针
    static ShaderQueueSuit_ComputeH0k*  sqsH0k;//h0(k)和 h0(-k)计算着色器管线指针
    static ShaderQueueSuit_ComputeHkt*  sqsHkt;//h(k,t)计算着色器管线指针
    static ShaderQueueSuit_ComputeInversion*  sqsInversion;//转换海面模型计算着色器管线指针
    static ShaderQueueSuit_ComputeTwiddleFactors*  sqsTwiddleFactors; //扰动因子计算着色器管线指针
    static ShaderQueueSuit_ComputeNormal*  sqsNormal;//法向量计算着色器管线指针
    static TexLightObject* waterForDraw;//海面绘制用对象指针
    static float choppiness;//波浪翻滚因子(x、z 轴扰动的放大系数)
    static float heightScale;//高度放大系数
    static float windX;//风向向量 X 分量
    static float windY;//风向向量 Y 分量
    static float windSpeed;//风速
    static int fftResolution;//fft 计算的尺寸
    static float fftAmplitude;//fft 计算的振幅常数
    static int fftL;//fft 计算中的 fftL 常数
    static float fftCapillarwavesSuppression;//fft 计算中的 Cap 常数
    static float time;//时间变量
    static float t_delta;//时间步进
    static int *horizontalPushConstants;//横向蝴蝶计算时的推送常量数据数组
    static int *verticalPushConstants;//竖向蝴蝶计算时的推送常量数据数组
    static int *inversionPushConstants;//转换海面模型时的推送常量数据数组
    static int log_2_n;//log2(n)常数，n 为 fft 计算的尺寸
    static int stages;//蝴蝶计算总阶段数，等于 log2(n)

    static void init_vulkan_instance();
    static void destroy_vulkan_instance();
    static void enumerate_vulkan_phy_devices();
    static void create_vulkan_devices();
    static void destroy_vulkan_devices();
    static void create_vulkan_CommandBuffer();
    static void destroy_vulkan_CommandBuffer();
    static void create_vulkan_swapChain();
    static void destroy_vulkan_swapChain();
    static void create_vulkan_DepthBuffer();
    static void destroy_vulkan_DepthBuffer();
    static void create_render_pass();
    static void destroy_render_pass();
    static void init_queue();
    static void create_frame_buffer();
    static void destroy_frame_buffer();
    static void createDrawableObject();
    static void destroyDrawableObject();
    static void calTaskNormal();
    static void calOceanFFTPara();
    static void drawTask();
    static void calTaskTwiddleFactors();
    static void calTaskH0kH0minusk();
    static void calTaskButterfly(std::string pingpong0, std::string pingpong1, int stage, int pingpong, int direction);
    static void calTaskInversion(int N, int pingpong);
    static void calTaskHkt();
    static void drawObject();
    static void doVulkan();
    static void create_vulkan_StorageBuffer(int index);//创建单个存储缓冲的方法
    static void destroy_vulkan_StorageBuffer(int index);//销毁单个存储缓冲的方法
    static void create_vulkan_StorageBuffers();//创建所有存储缓冲的方法
    static void destroy_vulkan_StorageBuffers();//销毁所有存储缓冲的方法
    static void init_texture();
    static void destroy_textures();
    static void initPipeline();
    static void destroyPipeline();
    static void createFence();
    static void destroyFence();
    static void initPresentInfo();
    static void initMatrixAndLight();
    static void flushUniformBufferForDraw();
    static void flushTexToDesSetForDraw();
    static void flushTexToDesSetForNormal();
};
#endif 
