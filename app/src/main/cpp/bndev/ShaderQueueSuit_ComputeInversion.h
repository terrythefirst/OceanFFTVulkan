#ifndef VULKANEXBASE_ShaderQueueSuit_ComputeInversion_H
#define VULKANEXBASE_ShaderQueueSuit_ComputeInversion_H

#include <vector>
#include <vulkan/vulkan.h>


class ShaderQueueSuit_ComputeInversion
{
private:
	int NUM_DESCRIPTOR_SETS;

	std::vector<VkDescriptorSetLayout> descLayouts;

	VkPipelineShaderStageCreateInfo shaderStages[1];

	VkPipelineCache pipelineCache;

	VkDevice* devicePointer;

	VkDescriptorPool descPool;
	void create_pipeline_layout(VkDevice& device);
	void destroy_pipeline_layout(VkDevice& device);
	void init_descriptor_set(VkDevice& device);
	void create_shader(VkDevice& device);
	void destroy_shader(VkDevice& device);
	void create_pipe_line(VkDevice& device);
	void destroy_pipe_line(VkDevice& device);

public:
	VkDeviceMemory memUniformBuf;
	int bufferByteCount;

	VkWriteDescriptorSet writes[5];

	std::vector<VkDescriptorSet> descSet;

	VkPipelineLayout pipelineLayout;

	VkPipeline pipeline;

	ShaderQueueSuit_ComputeInversion(VkDevice* deviceIn, VkPhysicalDeviceMemoryProperties& memoryroperties);
	~ShaderQueueSuit_ComputeInversion();
};

#endif
