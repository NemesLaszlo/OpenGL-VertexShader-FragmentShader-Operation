#version 130

in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_uv;

out vec4 fs_out_col;

uniform sampler2D texImage;

void main()
{
	fs_out_col = texture(texImage,vs_out_uv);
}