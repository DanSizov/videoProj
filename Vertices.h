#ifndef VERTICES_H
#define VERTCES_H

//���������� ������ �������������� ��� �������� + ���������� ��������
float verticesOriginal[] = {
	 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, // ������� ������ ����
	 1.0f, -1.0f, 0.0f,   1.0f, 1.0f, // ������ ������ ����
	-1.0f, -1.0f, 0.0f,   0.0f, 1.0f, // ������ ����� ����
	-1.0f,  1.0f, 0.0f,   0.0f, 0.0f  // ������� ����� ����
};

//����������� ����������� � ����� �������� ����
float verticesSmall[] = {
	 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, // ������� ����
	 0.0f,  0.0f, 0.0f,  1.0f, 1.0f, // �����
	-1.0f,  0.0f, 0.0f,  0.0f, 1.0f, // ����� ����
	-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, // ������� ����� ����
};

float verticesHalf1[] = {
	 0.0f,  1.0f, 0.0f,	 1.0f, 0.0f,
	 0.0f, -1.0f, 0.0f,	 1.0f, 1.0f,
	-1.0f, -1.0f, 0.0f,	 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f,	 0.0f, 0.0f
};

float verticesHalf2[] = {
	1.0f,  1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,	0.0f, 1.0f,
	0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
	0.0f,  1.0f, 0.0f,	1.0f, 0.0f
};

float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

#endif // !VERTICES_H

