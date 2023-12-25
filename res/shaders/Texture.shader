#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aTex;

out vec2 TexCoord;
out float TexImage;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	TexImage = aTex;
}

#shader fragment
#version 460 core
in vec2 TexCoord;
in float TexImage;

out vec4 FragColor;

// texture samplers
uniform sampler2D stone;
uniform sampler2D dirt;
uniform sampler2D grass;
uniform sampler2D water;
uniform sampler2D snow;

void main()
{
	if (TexImage == 1.0f)
		FragColor = texture(stone, TexCoord);
	else if (TexImage == 2.0f)
		FragColor = texture(dirt, TexCoord);
	else if (TexImage == 3.0f)
		FragColor = texture(grass, TexCoord);
	else if (TexImage == 4.0f)
		FragColor = vec4(1.0, 1.0, 1.0, 0.5) * texture(water, TexCoord);
	else if (TexImage == 5.0f)
		FragColor = texture(snow, TexCoord);
}
