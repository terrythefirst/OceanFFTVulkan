



#ifndef VULKANEXBASE_TEXTUREMANAGER_H
#define VULKANEXBASE_TEXTUREMANAGER_H

#include <vector>
#include <map>
#include <string>
#include <vulkan/vulkan.h>
#include "../vksysutil/vulkan_wrapper.h"
#include "TexDataObject.h"


#define SAMPLER_COUNT 1


class TextureManager
{
  public:
    
    static std::vector<std::string> texNames;
    
    static bool needStaging;
    
    static std::vector<VkSampler> samplerList;
    
    static std::map<std::string,VkImage> textureImageList;
    
    static std::map<std::string,VkDeviceMemory> textureMemoryList;
    
    static std::map<std::string,VkImage> textureImageListHelp;
    
    static std::map<std::string,VkDeviceMemory> textureMemoryListHelp;

    
    static std::map<std::string,VkImageView> viewTextureList;
    
    static std::map<std::string,VkDescriptorImageInfo> texImageInfoList;


    static void setImageLayout(VkCommandBuffer cmd, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout);

    static void init_COMPUTE_2D_Textures(std::string texName, VkDevice & device, VkPhysicalDevice & gpu, VkPhysicalDeviceMemoryProperties & memoryroperties, VkCommandBuffer & cmdBuffer, VkQueue & queueGraphics, VkFormat format, TexDataObject * ctdo);

    static void initSampler(VkDevice & device, VkPhysicalDevice & gpu);

    static void init_SPEC_2D_Textures(std::string texName, VkDevice & device, VkPhysicalDevice & gpu, VkPhysicalDeviceMemoryProperties & memoryroperties, VkCommandBuffer & cmdBuffer, VkQueue & queueGraphics, VkFormat format, TexDataObject * ctdo);

    static void initTextures(VkDevice& device, VkPhysicalDevice& gpu, VkPhysicalDeviceMemoryProperties& memoryroperties, VkCommandBuffer& cmdBuffer, VkQueue& queueGraphics);

    static void destroyTextures(VkDevice& device);

    static int getVkDescriptorSetIndexForCommonTexLight(std::string texName);
};


#endif //VULKANEXBASE_TEXTUREMANAGER_H
