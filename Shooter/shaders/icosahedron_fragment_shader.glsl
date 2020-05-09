#version 460

out vec4 FragmentColor;
in vec2 texCoords;
uniform sampler2D triangleTexture;
uniform bool colorFlag;

void main() {
    vec4 texColor =  texture(triangleTexture, texCoords);
    if(colorFlag) {
        texColor.xyz = vec3(1.0, 0.0, 0.0);
    }
    if(texColor.a < 0.1)
        discard;
    FragmentColor = texColor;
}
