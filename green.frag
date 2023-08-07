//����������� ������, ���������, ������������� ��� ������� �������, ���� ������ ���������� ���� ������� ������� � ����������� �� ����������, ���������� �� ���������� ������� � ������ ����������

#version 330 core

//���������� ��� �������� ������������ ����� ������� �������
out vec4 FragColorGreen;

//������� ���������� �� ���������� �������
in vec2 TexCoords;

//uniform ���������� ��������������� � ������������ ���������� � ����� ���� �������� ��� ������ �� ���� ��������, ������������ ��� ������� � ��������
uniform sampler2D screenTexture;
uniform float colorVariable;

void main()
{
//������� ����� �������� � �� ���������� � ���������� ���� �� �������� � ��������� �����������
    FragColorGreen = texture(screenTexture, TexCoords);
    FragColorGreen.r *= colorVariable;
    FragColorGreen.b *= colorVariable;

}
