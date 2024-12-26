// 代码来自于https://www.youtube.com/watch?v=p09i_hoFdd0
// 视频中用到的公式计算网站：https://zs.symbolab.com/solver/matrix-multiply-calculator

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> //支持文件操作、进程控制、环境管理、标准输入输出

float A, B, C;
float x, y, z;
float ooz;
int xp, yp;
int idx;


float cubeWidth = 20;
int width = 160, height = 44;
float zBuffer[160 * 44];
char buffer[160 * 44];
int backgroundASCIICode = ' ';
int distanceFromCam = 100;
float K1 = 40;


float incrementSpeed = 0.6;

float caculateX(int i, int j, int k){
	
	return j * sin(A) * sin(B) * cos(C) - k* cos(A) * sin(B) * cos(C) +
			j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
} 

float caculateY(int i, int j, int k){
	
	return j * cos(A) * cos(C) + k * sin(A) * cos(C) - j * sin(A) * sin(B) * sin(C) + 
			k * cos(A) * sin(B) * sin(C) - i * cos(B) * sin(C);
}

float caculateZ(int i, int j, int k){
	
	return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void caculateForSurface(float cubeX, float cubeY, float cubeZ, int ch){
	x = caculateX(cubeX, cubeY, cubeZ);
	y = caculateY(cubeX, cubeY, cubeZ);
	z = caculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;
	
	ooz = 1 / z;
	
	xp = (int)(width/2 - 2 * cubeWidth + K1 * ooz * x * 2);
	yp = (int)(height/2 + K1 * ooz * y);
	
	idx = xp + yp * width;
	if (idx >=0 && idx < width * height){
		if (ooz > zBuffer[idx]) {
			zBuffer[idx] = ooz;
			buffer[idx] = ch;
		}
	}
}



int main(){
	printf("\x1b[2J");
	while (1){
		memset(buffer, backgroundASCIICode, width * height);  // 初始化 
		memset(zBuffer, 0, width * height * 4);
		for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
			for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed){
				caculateForSurface(cubeX, cubeY, -cubeWidth, '#');
				caculateForSurface(cubeX, -cubeWidth, -cubeY, ';'); // 沿着x轴旋转90度。
				caculateForSurface(cubeX, cubeWidth, cubeY, '+');   // 沿着x轴旋转-90度。
				caculateForSurface(cubeWidth, cubeY, cubeX, '$');   // 沿着y轴旋转90度。
				caculateForSurface(-cubeWidth, cubeY, -cubeX, '~'); // 沿着y轴旋转-90度。
				caculateForSurface(cubeX, cubeY, cubeWidth, '.');   // 对面
				
			}
		}
		printf("\x1b[H");
		for (int k=0; k < width * height; k++){
			putchar(k % width ? buffer[k] : 10); //putchar : 将一个字符输出到标准输出设备
		}
		
		A += 0.005;
		B += 0.005;
		C += 0.005;
		usleep(100);
	}
	return 0;
}





























