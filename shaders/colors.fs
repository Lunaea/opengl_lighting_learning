#version 330 core
out vec4 FragColor;

in vec3 Lighting;

// uniform vec3 lightPos;
uniform vec3 objectColor;

void main()
{
    FragColor = vec4(Lighting * objectColor, 1.0f);
}
