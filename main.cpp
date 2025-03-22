#include<vulkan/vulkan.hpp>
#include<iostream>
#include<string>
#include<fstream>

vk::ShaderModule shaderFromFile(vk::Device device, std::string file){
    std::ifstream fileStream(file);
    std::stringstream stream;
    stream << fileStream.rdbuf();
    std::string shaderCode = stream.str();

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = shaderCode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode.data());

    return device.createShaderModule(shaderModuleCreateInfo);
}

int main(){
    // float a[1024];
    // // float b[1024];
    // for(int i = 0; i < 1024; i++){
    //     a[i] = 1.5;
    //     // b[i] = 2.5;
    // }

    
    std::cout << "Vulkan Compute Shader Runtime\n";

    vk::Instance instance;

    vk::ApplicationInfo appInfo;
    appInfo.sType = vk::StructureType::eApplicationInfo;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    vk::InstanceCreateInfo createInfo;
    createInfo.pApplicationInfo = &appInfo;

    instance = vk::createInstance(createInfo);
    auto devices = instance.enumeratePhysicalDevices();
    for(auto device:devices){
        std::cout << device.getProperties().deviceName << " " << device.getMemoryProperties().memoryHeaps[1].size/1000000000.0f << "\n";
        auto extensions = device.enumerateDeviceExtensionProperties();
        for(auto extension:extensions){
            std::cout << extension.extensionName << " " << extension.specVersion << "\n";
        }
        std::cout << "\n";
    }

    vk::PhysicalDevice physicalDevice = devices[0];
    int queueFamilyIndex = -1;
    auto queues = physicalDevice.getQueueFamilyProperties();
    for(long unsigned int queueIndex = 0; queueIndex < queues.size(); queueIndex++){
        if(queues[queueIndex].queueFlags & vk::QueueFlagBits::eCompute){
            queueFamilyIndex = queueIndex;
        }
    }

    vk::PhysicalDeviceFeatures physicalDeviceFeatures;

    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

    vk::Device device = physicalDevice.createDevice(deviceCreateInfo);
    vk::Queue queue = device.getQueue(queueFamilyIndex, 0);

    vk::ShaderModule shader = shaderFromFile(device, "shader.spv");

    vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo;
    pipelineShaderStageCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    pipelineShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eCompute;
    pipelineShaderStageCreateInfo.module = shader;
    pipelineShaderStageCreateInfo.pName = "main";

    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
    bufferCreateInfo.size = 1024*sizeof(float);
    vk::Buffer inBuffer = device.createBuffer(bufferCreateInfo);

    vk::MemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    memoryAllocateInfo.allocationSize = 1024*sizeof(float);
    vk::DeviceMemory inBufferMemory = device.allocateMemory(memoryAllocateInfo);
    device.bindBufferMemory(inBuffer, inBufferMemory, 0);

    // auto dataPtr = device.mapMemory(inBufferMemory, 0, 1024*sizeof(float));
    // memcpy(dataPtr, a, 1024*sizeof(float));
    // device.unmapMemory(inBufferMemory);

    // vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    // descriptorPoolCreateInfo.

    std::vector<vk::DescriptorSetLayoutBinding> bufferDescriptors(1);
    bufferDescriptors[0].binding = 0;
    bufferDescriptors[0].descriptorCount = 1;
    bufferDescriptors[0].descriptorType = vk::DescriptorType::eStorageBuffer;
    bufferDescriptors[0].stageFlags = vk::ShaderStageFlagBits::eCompute;

    vk::DescriptorSetLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.bindingCount = 1;
    layoutCreateInfo.pBindings = bufferDescriptors.data();
    vk::DescriptorSetLayout descriptorSetLayout = device.createDescriptorSetLayout(layoutCreateInfo);

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes(2);
    descriptorPoolSizes[0].descriptorCount = 1;
    descriptorPoolSizes[0].type = vk::DescriptorType::eStorageBuffer;

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets = 1;
    vk::DescriptorPool descriptorPool = device.createDescriptorPool(descriptorPoolCreateInfo);

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    vk::DescriptorSet descriptorSet = device.allocateDescriptorSets(descriptorSetAllocateInfo)[0];

    vk::DescriptorBufferInfo descriptorBufferInfo;
    descriptorBufferInfo.buffer = inBuffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = sizeof(inBuffer);

    std::vector<vk::WriteDescriptorSet> writeDescriptorSets(1);
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = vk::DescriptorType::eStorageBuffer;
    writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSets[0].dstSet = descriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].dstArrayElement = 0;
    writeDescriptorSets[0].descriptorType = vk::DescriptorType::eStorageBuffer;

    device.updateDescriptorSets(writeDescriptorSets, VK_NULL_HANDLE);

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    vk::PipelineLayout pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);

    // vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
    // pipelineCacheCreateInfo.initialDataSize

    vk::ComputePipelineCreateInfo computePipelineCreateInfo;
    computePipelineCreateInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
    computePipelineCreateInfo.stage = pipelineShaderStageCreateInfo;
    computePipelineCreateInfo.layout = pipelineLayout;
    vk::Pipeline pipeline = device.createComputePipeline(VK_NULL_HANDLE, computePipelineCreateInfo).value;

    vk::CommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    vk::CommandPool commandPool = device.createCommandPool(commandPoolCreateInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = 1;
    vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(commandBufferAllocateInfo)[0];
    
    std::vector<uint32_t> dynamicOffsets;
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBuffer.begin(commandBufferBeginInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, descriptorSet, {0});
    commandBuffer.dispatch(1024, 1 , 1);
    commandBuffer.end();
    
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::FenceCreateInfo fenceCreateInfo;
    vk::Fence fence = device.createFence(fenceCreateInfo);

    queue.submit({submitInfo}, fence);
    queue.waitIdle();

    auto data = device.mapMemory(inBufferMemory, 0, 1024*sizeof(float));
    for(int i = 0; i < 1024; i++){
        std::cout << ((float*)data)[i] << "\n";
    }
    return 0;
}