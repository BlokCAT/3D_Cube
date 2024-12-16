// BLACKCAT START 10 28
#include <iostream>
#include <string>
#include <Windows.h>
#include<cmath>
using namespace std;
const int W = 55;
int ChangeCorrd(int xx, int yy){return (W - yy) * W * 2 + (xx * 2 + (2* (W - yy))) - 1;}

float Z_buffer[W + 3][W + 3];
char* buffer = new char[((W) * ((W + 1) * 2)) + 1];
float angle = 0.07333 , angle2 = 0.03 , angle3 = 0.1;

void setPixel( int x , int y , char ch){ // x y 表示横坐标， 纵坐标
	buffer[ChangeCorrd(x, y)] = ch;
	buffer[ChangeCorrd(x, y) - 1] = ch;
}

class Vector3f {
public:
	float x = 0.0, y = 0.0, z = 0.0;
	Vector3f() {}
	Vector3f(float a, float b, float c) :x(a), y(b), z(c) {}
	Vector3f operator / (float r) const { return Vector3f(x / r, y / r, z / r); }
	Vector3f operator + (const Vector3f& v1) const { return Vector3f(x + v1.x, y + v1.y, z + v1.z); }
	Vector3f operator - (const Vector3f& v1) const { return Vector3f(x - v1.x, y - v1.y, z - v1.z); }
};
Vector3f crossProduct(const Vector3f& a, const Vector3f& b) { return Vector3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
struct Vector4f { 
	float x, y, z, w = 1.0; 
	Vector4f(float xx = 0.0f, float yy = 0.0f, float zz = 0.0f, float ww = 1.0f)
	: x(xx), y(yy), z(zz), w(ww) {}
};
Vector4f toVector4f(Vector3f v){return Vector4f {v.x , v.y , v.z , 1};}
Vector3f toVector3f(Vector4f v) { return Vector3f{ v.x , v.y , v.z}; }
class Matrix4D { //简单粗暴的矩阵类
public:
	Matrix4D() {}
	Matrix4D(float a11, float a12, float a13, float a14,
		float a21, float a22, float a23, float a24,
		float a31, float a32, float a33, float a34,
		float a41, float a42, float a43, float a44) {
		m[0][0] = a11; m[0][1] = a12; m[0][2] = a13; m[0][3] = a14;
		m[1][0] = a21; m[1][1] = a22; m[1][2] = a23; m[1][3] = a24;
		m[2][0] = a31; m[2][1] = a32; m[2][2] = a33; m[2][3] = a34;
		m[3][0] = a41; m[3][1] = a42; m[3][2] = a43; m[3][3] = a44;
	}
	Vector4f MVP(const Vector4f& R) const{ //变换矩阵和点向量的乘
		Vector4f result(0.0f, 0.0f, 0.0f, 0.0f);
		result.x = m[0][0] * R.x + m[0][1] * R.y + m[0][2] * R.z + m[0][3] * R.w;
		result.y = m[1][0] * R.x + m[1][1] * R.y + m[1][2] * R.z + m[1][3] * R.w;
		result.z = m[2][0] * R.x + m[2][1] * R.y + m[2][2] * R.z + m[2][3] * R.w;
		result.w = m[3][0] * R.x + m[3][1] * R.y + m[3][2] * R.z + m[3][3] * R.w;
		return result;
	}
	float m[4][4]; 
};
struct Face{//面类
	Face() {}
	Face(Vector3f a, Vector3f b, Vector3f c, Vector3f d):node1(a) , node2(b), node3(c), node4(d){Cen = (node1 + node3) / 2;} 
	Vector3f node1, node2, node3, node4 , Cen;
};
struct Cube {//立方体实例
	Face face[6];
	char color[6] = {'-' , '+' , '=' , '@' ,'*' ,'#'};
	Cube() {
		face[0] = Face(Vector3f(12, 12, 12), Vector3f(43, 12, 12), Vector3f(43, 43, 12), Vector3f(12, 43, 12));
		face[1] = Face(Vector3f(43, 12, 12), Vector3f(43, 12, 43), Vector3f(43, 43, 43), Vector3f(43, 43, 12));
		face[2] = Face(Vector3f(43, 12, 43), Vector3f(12, 12, 43), Vector3f(12, 43, 43), Vector3f(43, 43, 43));
		face[3] = Face(Vector3f(12, 12, 43), Vector3f(12, 12, 12), Vector3f(12, 43, 12), Vector3f(12, 43, 43));
		face[4] = Face(Vector3f(12, 12, 12), Vector3f(43, 12, 12), Vector3f(43, 12, 43), Vector3f(12, 12, 43));
		face[5] = Face(Vector3f(12, 43, 12), Vector3f(43, 43, 12), Vector3f(43, 43, 43), Vector3f(12, 43, 43));
	}	
	bool isHitScreenPlane(Vector3f pos, Vector3f node1, Vector3f node2, Vector3f node3, Vector3f node4){
		if (pos.x == node1.x || pos.y == node1.y )return false;
		Vector3f e1 = node2 - node1; Vector3f e2 = node3 - node2;Vector3f e3 = node4 - node3;Vector3f e4 = node1 - node4;
		Vector3f p1 = pos - node1;Vector3f p2 = pos - node2;Vector3f p3 = pos - node3;Vector3f p4 = pos - node4;
		Vector3f t1 = crossProduct(e1, p1);Vector3f t2 = crossProduct(e2, p2);Vector3f t3 = crossProduct(e3, p3);Vector3f t4 = crossProduct(e4, p4);
		if ((t1.z >= 0 && t2.z >= 0 && t3.z >= 0 && t4.z >= 0) || (t1.z <= 0 && t2.z <= 0 && t3.z <= 0 && t4.z <= 0))
			return true;
		return false;
	}
}cube;  
int main() {
	string t;	cout << "先ctrl+滚轮缩小控制台界面，建议全屏，再随便输入一个数字回车即可："; cin >> t; //随便输入一个东西， 输入前先缩放控制台，不然后面有撕裂
	while (1){
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
		cursorInfo.bVisible = false; // 隐藏光标
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
		memset(Z_buffer, 100, sizeof Z_buffer);
		/*填充画布为空格*/for (int k = 0; k < ((W) * ((W + 1) * 2)) + 1; k++) buffer[k] = ' ';
		/*换行*/    for (int y = 2; y <= W; y++) { buffer[ChangeCorrd(W + 1, y)] = '\n'; buffer[ChangeCorrd(W + 1, y) - 1] = ' '; }
		/*end字符*/ buffer[((W) * ((W + 1) * 2)) - 1] = '\0'; buffer[((W) * ((W + 1) * 2)) - 2] = ' ';

		Matrix4D TB(1, 0, 0, -27, 0, 1, 0, -27, 0, 0, 1, -27, 0, 0, 0, 1);
		Matrix4D TR(1, 0, 0, 27, 0, 1, 0, 27, 0, 0, 1, 27, 0, 0, 0, 1);
		Matrix4D Rx(1, 0, 0, 0, 0, cosf(angle3), -sinf(angle3), 0, 0, sinf(angle3), cosf(angle3), 0, 0, 0, 0, 1);
		Matrix4D Ry(cosf(-angle), 0, sinf(-angle), 0, 0, 1, 0, 0, -sinf(-angle), 0, cosf(-angle), 0, 0, 0, 0, 1);
		Matrix4D Rz(cosf(angle2), -sinf(angle2), 0, 0, sinf(angle2), cosf(angle2), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		for (int ii = 0; ii < 6; ii++) {
			cube.face[ii].node1 = toVector3f(TR.MVP(Rx.MVP(Ry.MVP(Rz.MVP(TB.MVP(toVector4f(cube.face[ii].node1)))))));
			cube.face[ii].node2 = toVector3f(TR.MVP(Rx.MVP(Ry.MVP(Rz.MVP(TB.MVP(toVector4f(cube.face[ii].node2)))))));
			cube.face[ii].node3 = toVector3f(TR.MVP(Rx.MVP(Ry.MVP(Rz.MVP(TB.MVP(toVector4f(cube.face[ii].node3)))))));
			cube.face[ii].node4 = toVector3f(TR.MVP(Rx.MVP(Ry.MVP(Rz.MVP(TB.MVP(toVector4f(cube.face[ii].node4)))))));
			cube.face[ii].Cen = toVector3f(TR.MVP(Rx.MVP(Ry.MVP(Rz.MVP(TB.MVP(toVector4f(cube.face[ii].Cen)))))));
		}
		for (int i = 0; i < 6; i++) //遍历cube的6个面 ， 光栅至屏幕
		{         //现在进行正交投影，其实只是取出x , y 坐标 , z默认1
			Vector3f toScreen1Node(cube.face[i].node1.x, cube.face[i].node1.y, 1);
			Vector3f toScreen2Node(cube.face[i].node2.x, cube.face[i].node2.y, 1);
			Vector3f toScreen3Node(cube.face[i].node3.x, cube.face[i].node3.y, 1);
			Vector3f toScreen4Node(cube.face[i].node4.x, cube.face[i].node4.y, 1);
			for (int x = 1; x <= W; x++) //遍历一个像素横坐标再纵坐标		
				for (int y = 1; y <= W; y++)  //看看有没有这这个面相交	
					if (cube.isHitScreenPlane(Vector3f(x, y, 1), toScreen1Node, toScreen2Node, toScreen3Node, toScreen4Node))
						if (Z_buffer[x][y] > cube.face[i].Cen.z) {
							setPixel(x, y, cube.color[i]);  //设立像素
							Z_buffer[x][y] = cube.face[i].Cen.z;  //更新深度缓存
						}
		}
		COORD coord = { 0, 0 };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
		fputs(buffer, stdout);
		Sleep(19);
	}
	return 0;
}
// BLACKCAT END 10 29
// BLACKCAT RELEASE 12 16
