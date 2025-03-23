#version 450
#extension GL_EXT_debug_printf : enable

layout (std140, binding = 0) buffer SSBO {
    float array[];
};

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

void main() 
{
    debugPrintfEXT("My float is %f", array[0]);
    return;
}