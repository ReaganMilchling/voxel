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
uniform sampler2D grass;
uniform sampler2D dirt;
uniform sampler2D stone;

void main()
{
	if (TexImage == 1.0f)
		FragColor = vec4(0.412, 0.412, 0.412, 1.0);
	else if (TexImage == 2.0f)
		FragColor = vec4(0.545, 0.271, 0.075, 1.0);
	else if (TexImage == 3.0f)
		FragColor = vec4(0.133, 0.545, 0.133, 1.0);
	else if (TexImage == 4.0f)
		FragColor = vec4(0.000, 0.000, 0.804, 0.5);
	else if (TexImage == 5.0f)
		FragColor = vec4(1.000, 0.980, 0.980, 1.0);
}
