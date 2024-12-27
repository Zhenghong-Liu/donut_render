#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// ����Ȧ����ģ��ʵ��
// ��render�ļ������޸ģ���������˹���ģ�͡�

float A=0, B=0, C=0; //��ת�Ƕ�
float x, y, z;  //��ת��ƽ�ƺ����ά����
float doughnutX, doughnutY, doughnutZ; //��ת��ƽ��֮ǰ����ά����
float ooz;  // ͸��ͶӰ
int xp, yp; //��Ⱦ�ڻ����ϵ�����
int idx;
float offsetX = 0;

float PI = 3.1415926535;
float R = 20;  //��Բ�뾶����z���Բ�İ뾶
float r = 8;   //СԲ�뾶�����ɻ����Բ�İ뾶

int width = 160, height = 44;
float zBuffer[160 * 44];     //ȷ���ڵ���ϵ
char buffer[160 * 44];       //��¼����ַ�
int backgroundASCIICode = ' ';
int distanceFromCam = 100;  //ƽ�Ƶľ��롣
float K1 = 50;  // ͸��ͶӰ������ϵ����������

// float incrementSpeed = 0.09; //ѭ�����
float incrementSpeed = 0.06; //ѭ�����

// ��Դλ��
float lightX = 20, lightY = -30, lightZ = -30;
float lightMagnitude;

// ��ʼ����Դ�ķ���
void initLight() {
    lightMagnitude = sqrt(lightX * lightX + lightY * lightY + lightZ * lightZ); //��һ��
    lightX /= lightMagnitude;
    lightY /= lightMagnitude;
    lightZ /= lightMagnitude;
}

// ���㷨�����ǿ
float calculateLightIntensity(float nx, float ny, float nz) {
    float magnitude = sqrt(nx * nx + ny * ny + nz * nz);  //��һ��
    nx /= magnitude;
    ny /= magnitude;
    nz /= magnitude;

    // �����������
    float dot = nx * lightX + ny * lightY + nz * lightZ;
    return dot > 0 ? dot : 0; // ��֤���ȷǸ�
}

float caculateX(float i, float j, float k){ //������ת��ʽ�Ƶ�
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
           j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
} 

float caculateY(float i, float j, float k){
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) - j * sin(A) * sin(B) * sin(C) + 
           k * cos(A) * sin(B) * sin(C) - i * cos(B) * sin(C);
}

float caculateZ(float i, float j, float k){
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void caculateForSurface(float dX, float dY, float dZ, float nx, float ny, float nz) {
	// dX,dY,dZ��ʾ��ά���꣬nx,ny,nz��ʾ��������
    x = caculateX(dX, dY, dZ) + offsetX;
    y = caculateY(dX, dY, dZ);
    z = caculateZ(dX, dY, dZ) + distanceFromCam; // ����ת��ƽ��

    // �Է�����������ת�� ����Ҫƽ��
    float rotate_nx = caculateX(nx, ny, nz);
    float rotate_ny = caculateY(nx, ny, nz);
    float rotate_nz = caculateZ(nx, ny, nz);

    ooz = 1 / z; //͸��ͶӰ�ı���

    xp = (int)(width / 2 - R + K1 * ooz * x * 2);  // x * 2��Ϊ�˺���ʾ��ƥ�䡣
    yp = (int)(height / 2 + K1 * ooz * y);

    idx = xp + yp * width;
    if (idx >= 0 && idx < width * height) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;  //zbuffer�㷨��ȷ���ڵ���ϵ

            // �����ǿ
            float intensity = calculateLightIntensity(rotate_nx, rotate_ny, rotate_nz); //��ȷ�Ĺ���
            // intensity = calculateLightIntensity(x, y, z - distanceFromCam); //������գ��е㲣�����ʵĸо�

            // ���ݹ�ǿѡ���ַ�
            // char luminance = "+*:;~=!@$&#"[(int)(intensity * 11)];
            char luminance = ".,-~:;=!*#$@"[(int)(intensity * 12)];
            buffer[idx] = luminance;
        }
    }
}

int main() {
    initLight();
    printf("\x1b[2J"); //����������ն��ϵ��������ݣ���
    while (1) {
        memset(buffer, backgroundASCIICode, width * height);  // ��ʼ��
        memset(zBuffer, 0, width * height * 4);  // float��Ҫ4���ֽ����洢

        for (float theta = 0; theta < 2 * PI; theta += incrementSpeed) {
            for (float phi = 0; phi < 2 * PI; phi += incrementSpeed) {
                // ����Ȧ����������
                doughnutX = (R + r * cos(phi)) * cos(theta);
                doughnutY = (R + r * cos(phi)) * sin(theta);
                doughnutZ = r * sin(phi);

                // ���㷨������
                float nx = cos(phi) * cos(theta); //������������Ƶ�
                float ny = cos(phi) * sin(theta);
                float nz = sin(phi);

                caculateForSurface(doughnutX, doughnutY, doughnutZ, nx, ny, nz); //��ͼ
            }
        }

        printf("\x1b[H"); //������ƶ����ն˵����Ͻ�
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? buffer[k] : '\n');  // ����ַ�, ÿһ�н�����Ҫ����س�
        }

        A += 0.009; // ������ת�ĽǶ�
        B += 0.005;
        C += 0.002;
        offsetX += 0.001;
        usleep(500);
    }
    return 0;
}
