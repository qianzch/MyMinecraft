#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
	vs_out.FragPos = vec3(model * vec4(pos, 1.0));
	vec3 normal = vec3(0.0, 0.0, 1.0);
	if(color.z == -1.0) {normal = vec3(0.0, 0.0, -1.0);}
	else if(color.z == 2.0) {normal = vec3(0.0, 1.0, 0.0);}
	else if(color.z == -2.0) {normal = vec3(0.0, -1.0, 0.0);}
	else if(color.z == 4.0) {normal = vec3(1.0, 0.0, 0.0);}
	else if(color.z == -4.0) {normal = vec3(-1.0, 0.0, 0.0);}
	vs_out.Normal = transpose(inverse(mat3(model))) * normal;
	
	vs_out.TexCoords = vec2(color.x, color.y);
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	gl_Position = projection * view * model * vec4(pos, 1.0);
}