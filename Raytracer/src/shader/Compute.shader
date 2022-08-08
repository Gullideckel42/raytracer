#version 460 core
layout(local_size_x = 4, local_size_y = 16, local_size_z = 1) in;
layout(rgba16f, binding = 0) uniform image2D out_tex;

void main()
{

	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	vec4 in_val = imageLoad(out_tex, pos).rgba;
	if (in_val == vec4(0, 0, 0, 1));
		imageStore(out_tex, pos, vec4(0.1, 0.1, 0.1, 1.0));

}