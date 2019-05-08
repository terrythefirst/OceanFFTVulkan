#ifndef VULKANEXBASE_ShaderQueueSuit_ComputeH0k_H
#define VULKANEXBASE_ShaderQueueSuit_ComputeH0k_H
#include <vector>
#include <vulkan/vulkan.h>
class ShaderQueueSuit_ComputeH0k
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
	VkWriteDescriptorSet writes[5];
	std::vector<VkDescriptorSet> descSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	ShaderQueueSuit_ComputeH0k(VkDevice* deviceIn, VkPhysicalDeviceMemoryProperties& memoryroperties);
	~ShaderQueueSuit_ComputeH0k();
};
#endif
