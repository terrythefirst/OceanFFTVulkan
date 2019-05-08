#ifndef VULKANEXBASE_TexLightObject_H
#define VULKANEXBASE_TexLightObject_H
#include <vulkan/vulkan.h>
#include "vulkan_wrapper.h"
#include <string>
#include "TextureManager.h"
class TexLightObject
{
public:
    VkDevice* devicePointer;
    float* vdata;
    int vCount;
    float* pushConstantData;
    int* indexData;
    int indexCount;
    VkBuffer vertexDatabuf;
    VkDeviceMemory vertexDataMem;
    VkDescriptorBufferInfo vertexDataBufferInfo;
    VkBuffer vertexDatabufCompute; 
    VkDeviceMemory vertexDataMemCompute; 
    VkDescriptorBufferInfo vertexDataBufferInfoCompute; 
    VkBuffer vertexIndexDatabuf; 
    VkDeviceMemory vertexIndexDataMem; 
    void createVertexDataBuffer(int dataByteCount, 
                                VkDevice& device,VkPhysicalDeviceMemoryProperties& memoryroperties);
    void createVertexDataBufferCompute(int dataByteCount, 
                                       VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryroperties);
    void createVertexIndexDataBuffer(int indexDataByteCount, 
                                     VkDevice& device,VkPhysicalDeviceMemoryProperties& memoryroperties);
    TexLightObject(float* vdataIn,int vdataByteCount,int vCountIn,int* indexDataIn,int indexDataByteCount,int indexCountIn,VkDevice& device,VkPhysicalDeviceMemoryProperties& memoryroperties);
    ~TexLightObject();
    void drawSelf(VkCommandBuffer& cmd, 
                  VkPipelineLayout& pipelineLayout,VkPipeline& pipeline,VkDescriptorSet* desSetPointer);
    void calSelfNormal(VkCommandBuffer& cmd, 
                       VkPipelineLayout &pipelineLayout, VkPipeline &pipeline,VkDescriptorSet *desSetPointer);
};
#endif
