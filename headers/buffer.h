void createBuffer(
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& bufferMemory
	) {
	VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) { throw std::runtime_error("failed to create buffer!"); }

	//Memory Requirements
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	//Memory Allocation
	VkMemoryAllocateInfo memoryAllocInfo{};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(device, &memoryAllocInfo, nullptr, &bufferMemory) != VK_SUCCESS) { throw std::runtime_error("failed to allocate buffer memory!"); }
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void createVertexBuffer() {
	VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer vertexStagingBuffer;
	VkDeviceMemory vertexStagingBufferMemory;

	//CPU visible buffer
	createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

	//Filling the Vertex Buffer
	void* data;
	vkMapMemory(device, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t) vertexBufferSize);
	vkUnmapMemory(device, vertexStagingBufferMemory);

	//GPU local buffer
	createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	//Transfer from CPU to GPU buffer
	copyBuffer(vertexStagingBuffer, vertexBuffer, vertexBufferSize);

	//Cleanup
	vkDestroyBuffer(device, vertexStagingBuffer, nullptr);
	vkFreeMemory(device, vertexStagingBufferMemory, nullptr);
}

void createIndexBuffer() {
	VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer indexStagingBuffer;
	VkDeviceMemory indexStagingBufferMemory;

	//CPU visible buffer
	createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexStagingBuffer, indexStagingBufferMemory);

	//Filling the Index Buffer
	void* indexData;
	vkMapMemory(device, indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
	memcpy(indexData, indices.data(), (size_t) indexBufferSize);
	vkUnmapMemory(device, indexStagingBufferMemory);

	//GPU local buffer
	createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	//Transfer from CPU to GPU buffer
	copyBuffer(indexStagingBuffer, indexBuffer, indexBufferSize);

	//Cleanup
	vkDestroyBuffer(device, indexStagingBuffer, nullptr);
	vkFreeMemory(device, indexStagingBufferMemory, nullptr);
}

void createUniformBuffer() {
	VkDeviceSize uniformBufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(
			uniformBufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i],
			uniformBuffersMemory[i]
			);
		vkMapMemory(device, uniformBuffersMemory[i], 0, uniformBufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkBufferCopy copyRegion{};
			copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	endSingleTimeCommands(commandBuffer);
}