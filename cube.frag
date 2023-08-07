//����������� ������, ���������, ������������� ��� ������� �������, ���� ������ ���������� ���� ������� ������� � ����������� �� ����������, ���������� �� ���������� ������� � ������ ����������

#version 330 core

uniform sampler2D textureSampler;

void main() {
    vec4 texColor = texture(textureSampler, texCoords);
    FragColor = texColor;
}