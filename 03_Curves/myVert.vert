#version 130

in vec3 vs_in_pos;
in vec3 vs_in_normal;

out vec3 vs_out_pos;
out vec3 vs_out_normal;
out vec2 vs_out_uv;

uniform mat4 world;
uniform mat4 worldIT;
uniform mat4 MVP;
uniform sampler2D texImage;

void main()
{
	const float PI = 3.1415926535897932384626433832795;

	float v = vs_in_pos.x;
	float u = vs_in_pos.z;

	float d = texture(texImage,vec2(u,v)).x;

	vs_out_pos = (1+d) * vec3(cos(PI*2*u)*sin(PI*v),cos(PI*v),sin(PI*2*u)*sin(PI*v));
	vs_out_normal = vec3(cos(PI*2*u)*sin(PI*v),cos(PI*v),sin(PI*2*u)*sin(PI*v));
	vs_out_uv = vec2(u,v);

	gl_Position = MVP * vec4( vs_out_pos, 1 );
	vs_out_pos = (world * vec4( vs_out_pos, 1 )).xyz;
	vs_out_normal  = (worldIT * vec4(vs_in_normal, 0)).xyz;


}