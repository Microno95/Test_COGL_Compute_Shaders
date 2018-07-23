#version 440 core
#extension GL_ARB_compute_shader : enable

struct vertex {
    vec3 pos;
    vec3 normal;
    vec4 colour;
    vec2 uv;
};

layout(std430, binding=1) buffer Particles {
	vertex ParticlesData[];
} data;

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

uvec3 size_mult = {1, gl_NumWorkGroups.x, gl_NumWorkGroups.x * gl_NumWorkGroups.y};
uint input_index = uint( gl_WorkGroupSize.x * dot(gl_WorkGroupID, size_mult) + gl_LocalInvocationID.x);
uint output_index = uint(gl_WorkGroupSize.y * input_index + gl_LocalInvocationID.y);

void main() {
	float a = 0.01f;
	vec3 rot_axis = {0.0f, 0.0f, 1.0f};

	vec3 pos;
	
	uint gid =  gl_GlobalInvocationID.x;
		
	if (gid < data.ParticlesData.length()) { 
		pos = data.ParticlesData[output_index].pos;
		pos = a * cross(rot_axis, pos) + pos;
		data.ParticlesData[output_index].pos = pos;
	}
}