#version 460 core
in vec2 TexCoord;
in float TexImage;

out vec4 FragColor;

// texture samplers
uniform sampler2D grass;
uniform sampler2D dirt;

void main()
{
	if (TexImage == 1.0f)
		FragColor = texture(grass, TexCoord);
	else
		FragColor = texture(dirt, TexCoord);
}