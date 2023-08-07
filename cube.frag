//фрагментный шейдер, программа, выполн€юща€с€ дл€ каждого пиксел€, этот шейдер определ€ет цвет каждого пиксел€ в зависимости от информации, переданной из вершинного шейдера и других источников

#version 330 core

uniform sampler2D textureSampler;

void main() {
    vec4 texColor = texture(textureSampler, texCoords);
    FragColor = texColor;
}