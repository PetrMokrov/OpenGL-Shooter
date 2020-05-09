#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 inTexCoords;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
out vec2 texCoords;

void main() {
    gl_Position = projectionTransform * viewTransform* modelTransform * vec4(position.x, position.y, position.z, 1.0f);
    texCoords = inTexCoords;
}
