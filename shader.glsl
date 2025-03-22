#version 450

layout (binding = 0) buffer SSBO {
    float array[];
};

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

void main() 
{
    uint index = gl_GlobalInvocationID.x;
    array[index] = index;
    return;
}