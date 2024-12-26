#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// ����Ȧ����ģ��ʵ��
// ��render�ļ������޸ģ���������˹���ģ�͡�

float A, B, C;
float x, y, z;
float doughnutX, doughnutY, doughnutZ;
float ooz;
int xp, yp;
int idx;

float PI = 3.1415926535;
float R = 20;
float r = 6;

int width = 160, height = 44;
float zBuffer[160 * 44];
char buffer[160 * 44];
int backgroundASCIICode = ' ';
int distanceFromCam = 100;
float K1 = 40;

float incrementSpeed = 0.09;

// ��Դλ��
float lightX = -1, lightY = 1, lightZ = -0.5;
float lightMagnitude;

// ��ʼ����Դ�ķ���
void initLight() {
    lightMagnitude = sqrt(lightX * lightX + lightY * lightY + lightZ * lightZ);
    lightX /= lightMagnitude;
    lightY /= lightMagnitude;
    lightZ /= lightMagnitude;
}

// ���㷨�����ǿ
float calculateLightIntensity(float nx, float ny, float nz) {
    float magnitude = sqrt(nx * nx + ny * ny + nz * nz);
    nx /= magnitude;
    ny /= magnitude;
    nz /= magnitude;

    // �����������
    float dot = nx * lightX + ny * lightY + nz * lightZ;
    return dot > 0 ? dot : 0; // ��֤���ȷǸ�
}

float caculateX(int i, int j, int k){
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
           j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
} 

float caculateY(int i, int j, int k){
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) - j * sin(A) * sin(B) * sin(C) + 
           k * cos(A) * sin(B) * sin(C) - i * cos(B) * sin(C);
}

float caculateZ(int i, int j, int k){
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void caculateForSurface(float dX, float dY, float dZ, float nx, float ny, float nz) {
	// dX,dY,dZ��ʾ��ά���꣬nx,ny,nz��ʾ��������
    x = caculateX(dX, dY, dZ);
    y = caculateY(dX, dY, dZ);
    z = caculateZ(dX, dY, dZ) + distanceFromCam;

    ooz = 1 / z;

    xp = (int)(width / 2 - 2 * R + K1 * ooz * x * 2);
    yp = (int)(height / 2 + K1 * ooz * y);

    idx = xp + yp * width;
    if (idx >= 0 && idx < width * height) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;

            // �����ǿ
            float intensity = calculateLightIntensity(nx, ny, nz);

            // ���ݹ�ǿѡ���ַ�
            char luminance = "~+*:;=!@$&#"[(int)(intensity * 11)];
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
                float nx = cos(phi) * cos(theta);
                float ny = cos(phi) * sin(theta);
                float nz = sin(phi);

                caculateForSurface(doughnutX, doughnutY, doughnutZ, nx, ny, nz); //��ͼ
            }
        }

        printf("\x1b[H"); //������ƶ����ն˵����Ͻ�
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? buffer[k] : 10);
        }

        A += 0.005; // ������ת�ĽǶ�
        B += 0.005;
        usleep(500);
    }
    return 0;
}
