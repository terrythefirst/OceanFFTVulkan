#ifndef VULKANEXBASE_ShaderQueueSuit_ComputeButterfly_H
#define VULKANEXBASE_ShaderQueueSuit_ComputeButterfly_H

#include <vector>
#include <vulkan/vulkan.h>


class ShaderQueueSuit_ComputeButterfly
{
private:
	int NUM_DESCRIPTOR_SETS;

	std::vector<VkDescriptorSetLayout> descLayouts;

	VkPipelineShaderStageCreateInfo shaderStages[1];

	VkPipelineCache pipelineCache;

	VkDevice* devicePointer;

	VkDescriptorPool descPool;
	void create_uniform_buffer(VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties);
	void destroy_uniform_buffer(VkDevice & device);
	void create_pipeline_layout(VkDevice& device);
	void destroy_pipeline_layout(VkDevice& device);
	void init_descriptor_set(VkDevice& device);
	void create_shader(VkDevice& device);
	void destroy_shader(VkDevice& device);
	void create_pipe_line(VkDevice& device);
	void destroy_pipe_line(VkDevice& device);

public:
	VkBuffer uniformBuf;
	VkDescriptorBufferInfo uniformBufferInfo;

	VkDeviceMemory memUniformBuf;
	int bufferByteCount;
	VkWriteDescriptorSet writes[4];

	std::vector<VkDescriptorSet> descSet;

	VkPipelineLayout pipelineLayout;

	VkPipeline pipeline;

	ShaderQueueSuit_ComputeButterfly(VkDevice* deviceIn, VkPhysicalDeviceMemoryProperties& memoryroperties);
	~ShaderQueueSuit_ComputeButterfly();
};

#endif
