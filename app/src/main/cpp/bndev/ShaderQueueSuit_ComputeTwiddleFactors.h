#ifndef VULKANEXBASE_ShaderQueueSuit_ComputeTwiddleFactors_H
#define VULKANEXBASE_ShaderQueueSuit_ComputeTwiddleFactors_H

#include <vector>
#include <vulkan/vulkan.h>
#include <math.h>


class ShaderQueueSuit_ComputeTwiddleFactors
{
private:
	int NUM_DESCRIPTOR_SETS;
	std::vector<VkDescriptorSetLayout> descLayouts;

	VkPipelineShaderStageCreateInfo shaderStages[1];

	VkPipelineCache pipelineCache;

	VkDevice* devicePointer;

	VkDescriptorPool descPool;
	void createBit_reversedDataBuffer(int dataByteCount, VkDevice & device, VkPhysicalDeviceMemoryProperties & memoryroperties);
	void initBitReversedIndices(int n);
	void destroy_uniform_buffer(VkDevice& device);
	void create_pipeline_layout(VkDevice& device);
	void destroy_pipeline_layout(VkDevice& device);
	void init_descriptor_set(VkDevice& device);
	void create_shader(VkDevice& device);
	void destroy_shader(VkDevice& device);
	void create_pipe_line(VkDevice& device);
	void destroy_pipe_line(VkDevice& device);



public:
	int *bitReversedIndices;
	VkBuffer bit_reversedBufCompute; //计算着色器和绘制共用的顶点数据缓冲
	VkDeviceMemory bit_reversedMemCompute; //第4 行顶点数据缓冲对应的设备内存
	VkDescriptorBufferInfo bit_reversedBufferInfoCompute; //第4 行顶点数据缓冲描述信息

	VkWriteDescriptorSet writes[2];

	std::vector<VkDescriptorSet> descSet;

	VkPipelineLayout pipelineLayout;

	VkPipeline pipeline;

	ShaderQueueSuit_ComputeTwiddleFactors(int n, VkDevice * deviceIn, VkPhysicalDeviceMemoryProperties & memoryroperties);
	~ShaderQueueSuit_ComputeTwiddleFactors();
};

#endif
