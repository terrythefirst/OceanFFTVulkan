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
    //Android app指针
    static android_app* Android_application;
    //vulkan绘制的循环标志
    static bool loopDrawFlag;
    //需要使用的扩展名称列表
    static std::vector<const char *> instanceExtensionNames;
    //vulkan实例
    static VkInstance instance;
    //物理设备数量
    static uint32_t gpuCount;
    //物理设备列表
    static  std::vector<VkPhysicalDevice> gpus;
    //物理设备对应的队列家族数量
    static uint32_t queueFamilyCount;
    //物理设备对应的队列家族列表
    static std::vector<VkQueueFamilyProperties> queueFamilyprops;
    //支持GRAPHICS工作的一个队列家族的索引
    static uint32_t queueGraphicsFamilyIndex;
    //支持Graphics的队列
    static VkQueue queueGraphics;
    //支持显示工作工作的一个队列家族的索引
    static uint32_t queuePresentFamilyIndex;
    //逻辑设备所需的扩展
    static std::vector<const char *> deviceExtensionNames;
    //创建的逻辑设备
    static VkDevice device;
    //创建的命令池
    static VkCommandPool cmdPool;
    //创建的一级命令缓冲
    static VkCommandBuffer cmdBuffer;
    //一级命令缓冲启动信息
    static VkCommandBufferBeginInfo cmd_buf_info;
    //供执行的命令缓冲数组
    static VkCommandBuffer cmd_bufs[1];
    //一级命令缓冲提交执行信息(第一个渲染任务用，第二个计算任务用)
    static VkSubmitInfo submit_info[2];
    //屏幕宽度
    static uint32_t screenWidth;
    //屏幕高度
    static uint32_t screenHeight;
    //创建的Surface
    static VkSurfaceKHR surface;
    //支持的格式
    static std::vector<VkFormat> formats;
    //surface的能力
    static VkSurfaceCapabilitiesKHR surfCapabilities;
    //显示模式数量
    static uint32_t presentModeCount;
    //显示模式列表
    static std::vector<VkPresentModeKHR> presentModes;
    //swapchain尺寸
    static VkExtent2D swapchainExtent;
    //创建的swapchain
    static VkSwapchainKHR swapChain;
    //swapchain中的Image数量
    static uint32_t swapchainImageCount;
    //swapchain中的Image列表
    static std::vector<VkImage> swapchainImages;
    //swapchain对应的的Image列表
    static std::vector<VkImageView> swapchainImageViews;
    //深度Image格式
    static VkFormat depthFormat;
    //物理设备支持的格式属性--为深度缓冲服务
    static VkFormatProperties depthFormatProps;
    //深度缓冲Image
    static VkImage depthImage;
    //物理设备内存属性
    static VkPhysicalDeviceMemoryProperties memoryroperties;
    //深度缓冲内存
    static VkDeviceMemory memDepth;
    //深度缓冲ImageView
    static VkImageView depthImageView;

    //渲染用Image准备完成信号量
    static VkSemaphore imageAcquiredSemaphore;
    //从swapChain中获取的当前渲染用Image的缓冲编号或称之为索引
    static uint32_t currentBuffer;
    //渲染通道
    static VkRenderPass renderPass;
    //渲染通道用清除帧缓冲数据
    static VkClearValue clear_values[2];
    //渲染通道启动信息
    static VkRenderPassBeginInfo rp_begin;
    //等待任务完毕Fence
    static VkFence taskFinishFence;
    //呈现信息
    static VkPresentInfoKHR present;

    //帧缓冲
    static VkFramebuffer* framebuffers;

    static VkBuffer storageBuffer[11]; //存储缓冲数组
    static VkDeviceMemory memStorageBuffer[11]; //存储缓冲对应的设备内存数组
    static VkDescriptorBufferInfo storageBufferInfo[11]; //存储缓冲信息
    static std::vector<std::string> storageBufferNames;
    static int getStorageBufferIndex(std::string ss);

    static ShaderQueueSuit_SingleTexLight* sqsSTL;
    static ShaderQueueSuit_ComputeButterfly*  sqsButterfly;
    static ShaderQueueSuit_ComputeH0k*  sqsH0k;
    static ShaderQueueSuit_ComputeHkt*  sqsHkt;
    static ShaderQueueSuit_ComputeInversion*  sqsInversion;
    static ShaderQueueSuit_ComputeTwiddleFactors*  sqsTwiddleFactors;
    static ShaderQueueSuit_ComputeNormal*  sqsNormal;


    static TexLightObject* waterForDraw;
    static float choppiness;
    static float distortion;
    static float wavemotion;
    static float heightScale;
    static float tessellationFactor;
    static float tessellationSlope;
    static float windX;
    static float windY;
    static float windSpeed;
    static int fftResolution;
    static float fftAmplitude;
    static int fftL;
    static float fftCapillarwavesSuppression;
    static float t_delta;

    static int *horizontalPushConstants;
    static int *verticalPushConstants;
    static int *inversionPushConstants;

    static int log_2_n;
    static int stages;
    static float time;

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
    static void create_vulkan_StorageBuffer(int index);
    static void destroy_vulkan_StorageBuffer(int index);
    static void create_vulkan_StorageBuffers();
    static void destroy_vulkan_StorageBuffers();
    static void init_texture();
    static void destroy_textures();

    static void initPipeline();
    static void destroyPipeline();

    static void createFence();
    static void destroyFence();

    static void initPresentInfo();

    static void initMatrixAndLight();

    static void flushUniformBufferForBD();
    static void flushUniformBufferForDraw();
    static void flushTexToDesSetForDraw();
    static void flushTexToDesSetForBD();
    static void flushTexToDesSetForNormal();
};


#endif //VULKANEXBASE_MYVULKANMANAGER_H
