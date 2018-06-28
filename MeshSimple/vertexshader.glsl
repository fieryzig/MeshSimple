#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	//vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	//vec3 lightDir = normalize(vec3(1, 1, 1));
	//float NL = max(dot(normal, lightDir), 0.0);
	//gl_Color = NL * vec4(1, 1, 1, 1);
}