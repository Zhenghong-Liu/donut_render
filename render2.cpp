#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// 甜甜圈光照模型实现
// 对render文件稍作修改，并且添加了光照模型。

float A, B, C; //旋转角度
float x, y, z;  //旋转和平移后的三维坐标
float doughnutX, doughnutY, doughnutZ; //旋转和平移之前的三维坐标
float ooz;  // 透视投影
int xp, yp; //渲染在画布上的坐标
int idx;

float PI = 3.1415926535;
float R = 20;  //大圆半径，绕z轴的圆的半径
float r = 6;   //小圆半径，生成环面的圆的半径

int width = 160, height = 44;
float zBuffer[160 * 44];     //确定遮挡关系
char buffer[160 * 44];       //记录输出字符
int backgroundASCIICode = ' ';
int distanceFromCam = 100;  //平移的距离。
float K1 = 50;  // 透视投影的缩放系数，代表焦距

float incrementSpeed = 0.09; //循环间隔

// 光源位置
float lightX = -1, lightY = 1, lightZ = -0.5;
float lightMagnitude;

// 初始化光源的方向
void initLight() {
    lightMagnitude = sqrt(lightX * lightX + lightY * lightY + lightZ * lightZ); //归一化
    lightX /= lightMagnitude;
    lightY /= lightMagnitude;
    lightZ /= lightMagnitude;
}

// 计算法线与光强
float calculateLightIntensity(float nx, float ny, float nz) {
    float magnitude = sqrt(nx * nx + ny * ny + nz * nz);  //归一化
    nx /= magnitude;
    ny /= magnitude;
    nz /= magnitude;

    // 点积计算亮度
    float dot = nx * lightX + ny * lightY + nz * lightZ;
    return dot > 0 ? dot : 0; // 保证亮度非负
}

float caculateX(int i, int j, int k){ //根据旋转公式推导
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
	// dX,dY,dZ表示三维坐标，nx,ny,nz表示法向量。
    x = caculateX(dX, dY, dZ);
    y = caculateY(dX, dY, dZ);
    z = caculateZ(dX, dY, dZ) + distanceFromCam; // 先旋转后平移

    ooz = 1 / z; //透视投影的比例

    xp = (int)(width / 2 - R + K1 * ooz * x * 2);  // x * 2是为了和显示器匹配。
    yp = (int)(height / 2 + K1 * ooz * y);

    idx = xp + yp * width;
    if (idx >= 0 && idx < width * height) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;  //zbuffer算法，确定遮挡关系

            // 计算光强
            float intensity = calculateLightIntensity(nx, ny, nz);

            // 根据光强选择字符
            char luminance = "~+*:;=!@$&#"[(int)(intensity * 11)];
            buffer[idx] = luminance;
        }
    }
}

int main() {
    initLight();
    printf("\x1b[2J"); //清屏（清除终端上的所有内容）。
    while (1) {
        memset(buffer, backgroundASCIICode, width * height);  // 初始化
        memset(zBuffer, 0, width * height * 4);  // float需要4个字节来存储

        for (float theta = 0; theta < 2 * PI; theta += incrementSpeed) {
            for (float phi = 0; phi < 2 * PI; phi += incrementSpeed) {
                // 甜甜圈参数化坐标
                doughnutX = (R + r * cos(phi)) * cos(theta);
                doughnutY = (R + r * cos(phi)) * sin(theta);
                doughnutZ = r * sin(phi);

                // 计算法线向量
                float nx = cos(phi) * cos(theta); //根据向量外积推导
                float ny = cos(phi) * sin(theta);
                float nz = sin(phi);

                caculateForSurface(doughnutX, doughnutY, doughnutZ, nx, ny, nz); //绘图
            }
        }

        printf("\x1b[H"); //将光标移动到终端的左上角
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? buffer[k] : '\n');  // 输出字符, 每一行结束都要输出回车
        }

        A += 0.005; // 更新旋转的角度
        B += 0.005;
        usleep(500);
    }
    return 0;
}
