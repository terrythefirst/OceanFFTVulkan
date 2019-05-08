#include "ShaderQueueSuit_ComputeTwiddleFactors.h"
#include <assert.h>
#include "../util/HelpFunction.h"
#include "../util/TextureManager.h"
#include "../util/FileUtil.h"
#include "MyVulkanManager.h"
#include "ShaderCompileUtil.h"
void ShaderQueueSuit_ComputeTwiddleFactors::createBit_reversedDataBuffer(int dataByteCount, VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties)
{//创建辅助数组对应缓冲
	VkBufferCreateInfo buf_info = {};//缓冲创建信息
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buf_info.pNext = NULL;
	buf_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;//设置用途
	buf_info.size = dataByteCount;//设置用途
	buf_info.queueFamilyIndexCount = 0;
	buf_info.pQueueFamilyIndices = NULL;
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buf_info.flags = 0;
	VkResult result = vk::vkCreateBuffer(device, &buf_info, NULL, &bit_reversedBufCompute);//创建缓冲
	assert(result == VK_SUCCESS);
	VkMemoryRequirements mem_reqs;
	vk::vkGetBufferMemoryRequirements(device, bit_reversedBufCompute, &mem_reqs);
	assert(dataByteCount <= mem_reqs.size);
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.pNext = NULL;
	alloc_info.memoryTypeIndex = 0;
	alloc_info.allocationSize = mem_reqs.size;
	VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; //给定类型掩码
	bool flag = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
	if (flag)
	{
		printf("确定内存类型成功 类型索引�?%d\n", alloc_info.memoryTypeIndex);
	}
	else
	{
		printf("确定内存类型失败!\n");
	}
	result = vk::vkAllocateMemory(device, &alloc_info, NULL, &bit_reversedMemCompute);
	assert(result == VK_SUCCESS);
	int *pData;
	int *cData = bitReversedIndices;
	result = vk::vkMapMemory(device, bit_reversedMemCompute, 0, mem_reqs.size, 0, (void **)&pData);
	assert(result == VK_SUCCESS);
	memcpy(pData, cData, dataByteCount);
	vk::vkUnmapMemory(device, bit_reversedMemCompute);
	result = vk::vkBindBufferMemory(device, bit_reversedBufCompute, bit_reversedMemCompute, 0);
	assert(result == VK_SUCCESS);
	bit_reversedBufferInfoCompute.buffer = bit_reversedBufCompute;//设置缓冲区信息的缓冲
	bit_reversedBufferInfoCompute.offset = 0;//设置缓冲区信息的偏移量
	bit_reversedBufferInfoCompute.range = mem_reqs.size;//设置缓冲区信息的字节数
}
unsigned int rotl(const unsigned int value, int shift) {//无符号左移的方法
	if ((shift &= sizeof(value) * 8 - 1) == 0)
		return value;
	return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}
uint32_t reverseBits(uint32_t n) {//整数比特左右颠倒的方法
	uint32_t res = 0;
	for (int i = 0; i < 32; ++i) {
		res |= ((n >> i) & 1) << (31 - i);
	}
	return res;
}
void ShaderQueueSuit_ComputeTwiddleFactors::initBitReversedIndices(int n) {//初始化辅助数组的方法
	bitReversedIndices = (int*)malloc(sizeof(int)*n);//申请空间
	int bits = (int)(log(n) / log(2));//左移的位数
	for (int i = 0; i<n; i++)//循环计算各位置的索引信息
	{
		int x = reverseBits(i);//将 i 进行比特的左右颠倒得到 x
		x = rotl(x, bits);//将 x 无符号左移
		bitReversedIndices[i] = x;//记录当前位置对应的索引
	}
}
void ShaderQueueSuit_ComputeTwiddleFactors::destroy_uniform_buffer(VkDevice& device)
{
	vk::vkDestroyBuffer(device, bit_reversedBufCompute, NULL);
	vk::vkFreeMemory(device, bit_reversedMemCompute, NULL);
}
void ShaderQueueSuit_ComputeTwiddleFactors::create_pipeline_layout(VkDevice& device)
{
	NUM_DESCRIPTOR_SETS=1;
	VkDescriptorSetLayoutBinding layout_bindings[2];
	layout_bindings[0].binding = 0;
	layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	layout_bindings[0].descriptorCount = 1;
	layout_bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	layout_bindings[0].pImmutableSamplers = NULL;
	layout_bindings[1].binding = 1;
	layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	layout_bindings[1].descriptorCount = 1;
	layout_bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	layout_bindings[1].pImmutableSamplers = NULL;
	VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
	descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_layout.pNext = NULL;
	descriptor_layout.bindingCount = 2;
	descriptor_layout.pBindings = layout_bindings;
	descLayouts.resize(NUM_DESCRIPTOR_SETS);
	VkResult result = vk::vkCreateDescriptorSetLayout(device, &descriptor_layout, NULL, descLayouts.data());
	assert(result == VK_SUCCESS);
	const unsigned push_constant_range_count = 1;
	VkPushConstantRange push_constant_ranges[push_constant_range_count] = {};
	push_constant_ranges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	push_constant_ranges[0].offset = 0;
	push_constant_ranges[0].size = sizeof(int) * 1;
	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pPipelineLayoutCreateInfo.pNext = NULL;
	pPipelineLayoutCreateInfo.pushConstantRangeCount = push_constant_range_count;
	pPipelineLayoutCreateInfo.pPushConstantRanges = push_constant_ranges;
	pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
	pPipelineLayoutCreateInfo.pSetLayouts = descLayouts.data();
	result = vk::vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, NULL, &pipelineLayout);
	assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_ComputeTwiddleFactors::destroy_pipeline_layout(VkDevice& device)
{
	vk::vkDestroyPipelineLayout(device, pipelineLayout, NULL);
}
void ShaderQueueSuit_ComputeTwiddleFactors::init_descriptor_set(VkDevice& device)
{
	VkDescriptorPoolSize type_count[2];
	type_count[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	type_count[0].descriptorCount = 1;
	type_count[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	type_count[1].descriptorCount = 1;
	VkDescriptorPoolCreateInfo descriptor_pool = {};
	descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool.pNext = NULL;
	descriptor_pool.maxSets = 1;
	descriptor_pool.poolSizeCount = 2;
	descriptor_pool.pPoolSizes = type_count;
	VkResult result = vk::vkCreateDescriptorPool(device, &descriptor_pool, NULL, &descPool);
	assert(result == VK_SUCCESS);
	std::vector<VkDescriptorSetLayout> layouts;
	for (int i = 0; i<1; i++)
	{
		layouts.push_back(descLayouts[0]);
	}
	VkDescriptorSetAllocateInfo alloc_info[1];
	alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info[0].pNext = NULL;
	alloc_info[0].descriptorPool = descPool;
	alloc_info[0].descriptorSetCount = 1;
	alloc_info[0].pSetLayouts = layouts.data();
	descSet.resize(1);
	result = vk::vkAllocateDescriptorSets(device, alloc_info, descSet.data());
	assert(result == VK_SUCCESS);
	writes[0] = {};
	writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[0].pNext = NULL;
	writes[0].descriptorCount = 1;
	writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	writes[0].dstArrayElement = 0;
	writes[0].dstBinding = 0;
	writes[1] = {};
	writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[1].pNext = NULL;
	writes[1].descriptorCount = 1;
	writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	writes[1].dstArrayElement = 0;
	writes[1].dstBinding = 1;
}
void ShaderQueueSuit_ComputeTwiddleFactors::create_shader(VkDevice& device)
{
	std::string compStr = FileUtil::loadAssetStr("shader/twiddleFactors.comp");
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].pNext = NULL;
	shaderStages[0].pSpecializationInfo = NULL;
	shaderStages[0].flags = 0;
	shaderStages[0].stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStages[0].pName = "main";
	std::vector<unsigned int> vtx_spv;
	bool retVal = GLSLtoSPV(VK_SHADER_STAGE_COMPUTE_BIT, compStr.c_str(), vtx_spv);
	assert(retVal);
	printf("计算�?色器脚本编译SPV成功！\n");
	VkShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
	moduleCreateInfo.pCode = vtx_spv.data();
	VkResult result = vk::vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderStages[0].module);
	assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_ComputeTwiddleFactors::destroy_shader(VkDevice& device)
{
	vk::vkDestroyShaderModule(device, shaderStages[0].module, NULL);
}
void ShaderQueueSuit_ComputeTwiddleFactors::create_pipe_line(VkDevice& device)
{
	VkComputePipelineCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = NULL;
	pipelineInfo.flags = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = 0;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.stage = shaderStages[0];
	VkPipelineCacheCreateInfo pipelineCacheInfo;
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheInfo.pNext = NULL;
	pipelineCacheInfo.initialDataSize = 0;
	pipelineCacheInfo.pInitialData = NULL;
	pipelineCacheInfo.flags = 0;
	VkResult result = vk::vkCreatePipelineCache(device, &pipelineCacheInfo, NULL, &pipelineCache);
	assert(result == VK_SUCCESS);
	result = vk::vkCreateComputePipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &pipeline);
	assert(result == VK_SUCCESS);
}
void ShaderQueueSuit_ComputeTwiddleFactors::destroy_pipe_line(VkDevice& device)
{
	vk::vkDestroyPipeline(device, pipeline, NULL);
	vk::vkDestroyPipelineCache(device, pipelineCache, NULL);
}
ShaderQueueSuit_ComputeTwiddleFactors::ShaderQueueSuit_ComputeTwiddleFactors(int n,VkDevice* deviceIn, VkPhysicalDeviceMemoryProperties& memoryroperties)
{
	this->devicePointer = deviceIn;
	initBitReversedIndices(n);
	createBit_reversedDataBuffer(sizeof(int)*n, *deviceIn, memoryroperties);
	create_shader(*devicePointer);
	create_pipeline_layout(*devicePointer);
	init_descriptor_set(*devicePointer);
	create_pipe_line(*devicePointer);
}
ShaderQueueSuit_ComputeTwiddleFactors::~ShaderQueueSuit_ComputeTwiddleFactors()
{
	destroy_pipe_line(*devicePointer);
	destroy_shader(*devicePointer);
	destroy_pipeline_layout(*devicePointer);
	destroy_uniform_buffer(*devicePointer);
}
