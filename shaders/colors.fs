#version 330 core
out vec4 FragColor;

in vec3 lighting;

// uniform vec3 lightPos;

void main()
{
    FragColor = vec4(lighting, 1.0f);
}
