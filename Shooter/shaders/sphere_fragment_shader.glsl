#version 460

out vec4 FragmentColor;
in vec2 texCoords;
uniform sampler2D sphereTexture;

void main() {
    FragmentColor = texture(sphereTexture, texCoords);
}
