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

    // vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    // descriptorPoolCreateInfo.

    vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding;
    descriptorSetLayoutBinding.binding = 0;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.descriptorType = vk::DescriptorType::e

    vk::DescriptorSetLayoutCreateInfo descriptorSetCreateInfo;
    descriptorSetCreateInfo.pBindings

    device.createDescriptorSetLayout()

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.pSetLayouts = 

    vk::ComputePipelineCreateInfo computePipelineCreateInfo;
    computePipelineCreateInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
    computePipelineCreateInfo.stage = pipelineShaderStageCreateInfo;
    computePipelineCreateInfo.layout = 

    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
    pipelineCacheCreateInfo.

    auto computePipeline = device.createComputePipeline(device.createPipelineCache())
    return 0;
}