#include <Windows.h>
#include "glut.h"
#include <stdio.h>
#include <string>
#include "SOIL.h"
#include <math.h>
#pragma comment(lib, "SOIL.lib")
#pragma warning(disable:4996)
#include <string>
using namespace std;
//#pragma comment (linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#define KEY_ESC            27
#define SELECT_BUF_SIZE      512
#define MAX_OBJECT         2
#define CUBE1         102
#define CUBE2         103
#define CUBE3         104
#define CUBE4			105
#define CUBE5			106
#define CUBE6			107
#define CUBE7			108
#define CUBE8			109
#define CUBE9			110
#define CUBE10			111
#define CUBE11			112
#define CUBE12			113
#define CUBE13			114
#define CUBE14			115
#define CUBE15			116
#define CUBE16			117

short g_nSelectedObject = CUBE1;
float g_fStartX = 0.0, g_fStartY = 0.0, g_fStartZ = 0.0;
float x_ctran = 0.0;
float y_angle = 0.0;

void Reshape(int w, int h);
void Display(void);
void MyKeyboard(unsigned char ucKey, int x, int y);
void Mouse(int iButton, int iState, int x, int y);
void Picking(int x, int y);
void ProcessHits(unsigned int uiHits, unsigned int *pBuffer);

class Cube {
public:
	int point_type = 0, dir_type = 0, spot_type = 0;
	int view_type = 0;

	float pnv[100][3];//polygon normal vector
	float vnv2[200][3];
	//LIGHT0 point light
	float light0_pos[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light0_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light0_diffuse[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light0_specular[4] = { 0.0, 0.0, 0.0, 1.0 };
	//LIGHT1 directional light
	float light1_dir[4] = { 0.0, 0.0, 0.0, 0.0 };
	float light1_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light1_diffuse[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light1_specular[4] = { 0.0, 0.0, 0.0, 1.0 };
	//LIGHT2 spotlight
	float light2_pos[4] = { 0.0, 0.0, 0.0, 0.2f };
	float light2_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light2_diffuse[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light2_specular[4] = { 0.0, 0.0, 0.0, 1.0 };
	float light2_dir[3] = { 0.0, 0.0, 0.0 };
	float light2_cutoff[1];
	//material Property
	float mat_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
	float mat_diffuse[4] = { 0.0, 0.0, 0.0, 1.0 };
	float mat_specular[4] = { 0.0, 0.0, 0.0, 1.0 };
	float mat_shininess[1];
	float mss = 0.078125 * 128;

	float x_angle[30] = { 0.0 }; // x축 회전 각도
	float y_angle[30] = { 0.0 }; // y축 회전 각도
	float z_angle[30] = { 0.0 }; // z축 회전각도
	float scale = 1.0; // 물체 축소 확대 값
	float x_tran[30] = { 0.0 };
	float y_tran[30] = { 0.0 };
	float z_tran[30] = { 0.0 };
	int v_index = 1;
	int f_index = 0;
	int v_index2 = 1;
	int f_index2 = 0;
	int fmt2[150][4];
	float vertex2[242][3];
	float vertex[100][3];
	int fmt[8][4];//사각형 face 순서를 담을 배열
	double tex_coord[4][8] = {  { 0.0,0.0,0.5,0.0,0.5,0.5,0.0,0.5 },
								{ 0.5,0.0, 1.0,0.0, 1.0,0.5, 0.5,0.5 },
								{ 0.0,0.5, 0.5,0.5, 0.5,1.0, 0.0,1.0 },
								{ 0.0,0.5, 0.5,0.5, 0.5,1.0, 0.0,1.0 } };
	unsigned int tex_names[30];

	void obj_read() {// 매개변수로 파일이름을 받아 파일을 읽음
		FILE* obj_file;
		char ch;

		obj_file = fopen("cube2.obj", "r");//파일을 읽기전용으로 오픈
		if (obj_file == NULL) {//파일이 존재하지 않을 시
			printf("Read error : File is not exist");//에러 메시지 출력
			return;
		}
		while (!feof(obj_file)) {//파일데이터를 반복문으로 읽음
			fscanf(obj_file, "%c", &ch);//데이터를 char 변수 ch에 임시저장
			if (!strncmp("#", &ch, 1)) {//앞자리가 #일 경우 주석처리
				continue;
			}
			if (ch == 'v') {//앞자리가 v일 경우 vertex 배열에 배정
				fscanf_s(obj_file, "%f %f %f", &vertex[v_index][0], &vertex[v_index][1], &vertex[v_index][2]);
				v_index++;//vertex 인덱스 1 증가
			}
			else if (ch == 'f') {//앞자리가 f일경우 fmt 배열에 배정
				fscanf_s(obj_file, "%d %d %d %d", &fmt[f_index][0], &fmt[f_index][1], &fmt[f_index][2], &fmt[f_index][3]);
				//printf("%d %d %d %d\n", fmt[f_index][0], fmt[f_index][1], fmt[f_index][2], fmt[f_index][3]);
				f_index++;//face 인덱스 1 증가
			}
		}
		fclose(obj_file);//파일 닫기
	}


	void obj_read2() {// 매개변수로 파일이름을 받아 파일을 읽음
		FILE* obj_file;
		char ch;

		obj_file = fopen("table2.obj", "r");//파일을 읽기전용으로 오픈
		if (obj_file == NULL) {//파일이 존재하지 않을 시
			printf("Read error : table is not exist");//에러 메시지 출력
			return;
		}
		while (!feof(obj_file)) {//파일데이터를 반복문으로 읽음
			fscanf(obj_file, "%c", &ch);//데이터를 char 변수 ch에 임시저장
			if (!strncmp("#", &ch, 1)) {//앞자리가 #일 경우 주석처리
				continue;
			}
			if (ch == 'v') {//앞자리가 v일 경우 vertex 배열에 배정
				fscanf_s(obj_file, "%f %f %f", &vertex2[v_index2][0], &vertex2[v_index2][1], &vertex2[v_index2][2]);
				//printf("%f %f %f \n", vertex2[v_index2][0], vertex2[v_index2][1], vertex2[v_index2][2]);
				v_index2++;//vertex 인덱스 1 증가
			}
			else if (ch == 'f') {//앞자리가 f일경우 fmt 배열에 배정
				fscanf_s(obj_file, "%d %d %d %d", &fmt2[f_index2][0], &fmt2[f_index2][1], &fmt2[f_index2][2], &fmt2[f_index2][3]);
				//printf("%d %d %d %d\n", fmt2[f_index2][0], fmt2[f_index2][1], fmt2[f_index2][2], fmt2[f_index2][3]);
				f_index2++;//face 인덱스 1 증가
			}
		}
		fclose(obj_file);//파일 닫기
	}
	void SetupTexture() {
		int width, height;
		GLubyte *image = SOIL_load_image("texture1.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLubyte *image2 = SOIL_load_image("texture2.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLubyte *image3 = SOIL_load_image("texture3.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLubyte *image4 = SOIL_load_image("texture4.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLubyte *image5 = SOIL_load_image("texture5.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLubyte *image6 = SOIL_load_image("texture6.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLubyte *image7 = SOIL_load_image("texture7.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLubyte *image8 = SOIL_load_image("texture8.png", &width, &height, 0, SOIL_LOAD_RGB);

		glGenTextures(9, tex_names);//2개 이미지 가져옴
		glBindTexture(GL_TEXTURE_2D, tex_names[0]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		glBindTexture(GL_TEXTURE_2D, tex_names[1]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);


		glBindTexture(GL_TEXTURE_2D, tex_names[2]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);

		glBindTexture(GL_TEXTURE_2D, tex_names[3]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);

		glBindTexture(GL_TEXTURE_2D, tex_names[4]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);

		glBindTexture(GL_TEXTURE_2D, tex_names[5]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image6);

		glBindTexture(GL_TEXTURE_2D, tex_names[6]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image7);

		glBindTexture(GL_TEXTURE_2D, tex_names[7]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image8);

		glEnable(GL_TEXTURE_2D);

		SOIL_free_image_data(image);
		SOIL_free_image_data(image2);
		SOIL_free_image_data(image3);
		SOIL_free_image_data(image4);
		SOIL_free_image_data(image5);
		SOIL_free_image_data(image6);
		SOIL_free_image_data(image7);
		SOIL_free_image_data(image8);
	}

	void myflat(float fStartX, float fStartY, float fStartZ) {// flat shading을 적용해주는 함수
		polygon_normal_vector();//다각형의 법선백터 계산 함수 실행

		glBegin(GL_QUADS);
		for (int i = 0; i < f_index; i++) {
			glNormal3f(pnv[i][0], pnv[i][1], pnv[i][2]);//삼각형의 법선백터를 넣어 줌		

			glTexCoord2f(tex_coord[i % 4][0], tex_coord[i % 4][1]);//텍스처 맵핑 위치 정해줌
			glVertex3f(fStartX + vertex[fmt[i][0]][0], fStartY + vertex[fmt[i][0]][1], fStartZ + vertex[fmt[i][0]][2]);

			glTexCoord2f(tex_coord[i % 4][2], tex_coord[i % 4][3]);
			glVertex3f(fStartX + vertex[fmt[i][1]][0], fStartY + vertex[fmt[i][1]][1], fStartZ + vertex[fmt[i][1]][2]);

			glTexCoord2f(tex_coord[i % 4][4], tex_coord[i % 4][5]);
			glVertex3f(fStartX + vertex[fmt[i][2]][0], fStartY + vertex[fmt[i][2]][1], fStartZ + vertex[fmt[i][2]][2]);

			glTexCoord2f(tex_coord[i % 4][6], tex_coord[i % 4][7]);
			glVertex3f(fStartX + vertex[fmt[i][3]][0], fStartY + vertex[fmt[i][3]][1], fStartZ + vertex[fmt[i][3]][2]);
		}
		glEnd();
	}

	void myflat2() {// flat shading을 적용해주는 함수
		vertex_normal_vector2();
		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_FILL);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
		for (int i = 0; i < f_index2; i++) {
			//glNormal3f(pnv[i][0], pnv[i][1], pnv[i][2]);//삼각형의 법선백터를 넣어 줌         
			//printf("%d\n", vnv2[fmt2[i][3]][3]);
			glNormal3f(vnv2[fmt2[i][3]][0] / 4, vnv2[fmt2[i][3]][1] / 4, vnv2[fmt2[i][3]][2] / 4);
			glVertex3f(vertex2[fmt2[i][3]][0], vertex2[fmt2[i][3]][1], vertex2[fmt2[i][3]][2]);
			glNormal3f(vnv2[fmt2[i][2]][0] / 4, vnv2[fmt2[i][2]][1] / 4, vnv2[fmt2[i][2]][2] / 4);
			glVertex3f(vertex2[fmt2[i][2]][0], vertex2[fmt2[i][2]][1], vertex2[fmt2[i][2]][2]);
			glNormal3f(vnv2[fmt2[i][1]][0] / 4, vnv2[fmt2[i][1]][1] / 4, vnv2[fmt2[i][1]][2] / 4);
			glVertex3f(vertex2[fmt2[i][1]][0], vertex2[fmt2[i][1]][1], vertex2[fmt2[i][1]][2]);
			glNormal3f(vnv2[fmt2[i][0]][0] / 4, vnv2[fmt2[i][0]][1] / 4, vnv2[fmt2[i][0]][2] / 4);
			glVertex3f(vertex2[fmt2[i][0]][0], vertex2[fmt2[i][0]][1], vertex2[fmt2[i][0]][2]);
			//printf("%d\n", vnv2[fmt2[i][0]][3]);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
	}

	void Draw(GLenum eMode)
	{
		if (eMode == GL_SELECT) glLoadName(CUBE1);
		if (CUBE1 == g_nSelectedObject) {
			DrawCube1(g_fStartX, g_fStartY, g_fStartZ);

		}
		else DrawCube1(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE2);
		if (CUBE2 == g_nSelectedObject) DrawCube2(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube2(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE3);
		if (CUBE3 == g_nSelectedObject) DrawCube3(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube3(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE4);
		if (CUBE4 == g_nSelectedObject) DrawCube4(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube4(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE5);
		if (CUBE5 == g_nSelectedObject) DrawCube5(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube5(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE6);
		if (CUBE6 == g_nSelectedObject) DrawCube6(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube6(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE7);
		if (CUBE7 == g_nSelectedObject) DrawCube7(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube7(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE8);
		if (CUBE8 == g_nSelectedObject) DrawCube8(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube8(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE9);
		if (CUBE9 == g_nSelectedObject) DrawCube9(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube9(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE10);
		if (CUBE10 == g_nSelectedObject) DrawCube10(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube10(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE11);
		if (CUBE11 == g_nSelectedObject) DrawCube11(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube11(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE12);
		if (CUBE12 == g_nSelectedObject) DrawCube12(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube12(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE13);
		if (CUBE13 == g_nSelectedObject) DrawCube13(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube13(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE14);
		if (CUBE14 == g_nSelectedObject) DrawCube14(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube14(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE15);
		if (CUBE15 == g_nSelectedObject) DrawCube15(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube15(0, 0, 0);

		if (eMode == GL_SELECT) glLoadName(CUBE16);
		if (CUBE16 == g_nSelectedObject) DrawCube16(g_fStartX, g_fStartY, g_fStartZ);
		else DrawCube16(0, 0, 0);


	}
	void shadow(float myx, float myy, float myz) {
		glPushMatrix();
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		//glEnable(GL_COLOR_MATERIAL);
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);//그림자
		glVertex3f(myx + 0.50f, myy + -0.5f, myz + 0.3f);
		glVertex3f(myx + 0.8f, myy + -0.5f, myz + 0.0f);
		glVertex3f(myx + 0.8f, myy + -0.5f, myz + -1.3f);
		glVertex3f(myx + 0.5f, myy + -0.5f, myz + -1.3f);
		glVertex3f(myx - 0.25f, myy + -0.5f, myz + -0.5f);
		glEnd();
		//glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glPopMatrix();

	}
	void DrawCubeLine(float fStartX, float fStartY, float fStartZ)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(9.0);
		glBegin(GL_QUADS);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(fStartX + -0.50f, fStartY + 0.5f, fStartZ + 0.5f);
		glVertex3f(fStartX + 0.50f, fStartY + 0.5f, fStartZ + 0.5f);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(fStartX + 0.50f, fStartY + -0.5f, fStartZ + 0.5f);
		glVertex3f(fStartX + -0.50f, fStartY + -0.5f, fStartZ + 0.5f);
		glEnd();
		glLineWidth(0.1);
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	void DrawCube1(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[0]);
		glTranslatef(-5.0, 0.0, 0.0);
		glTranslatef(x_tran[0], 0.0, 0.0);
		glTranslatef(0.0, y_tran[0], 0.0);
		glTranslatef(0.0, 0.0, z_tran[0]);

		shadow(fStartX, fStartY, fStartZ);

		glRotatef(y_angle[0], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[0], 1.0, 0.0, 0.0);
		glRotatef(z_angle[0], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE1 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}


	void DrawCube2(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[1]);
		glTranslatef(1.0, 0.0, 0.0);
		glTranslatef(x_tran[1], 0.0, 0.0);
		glTranslatef(0.0, y_tran[1], 0.0);
		glTranslatef(0.0, 0.0, z_tran[1]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[1], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[1], 1.0, 0.0, 0.0);
		glRotatef(z_angle[1], 0.0, 0.0, 1.0);
		myflat(fStartX, fStartY, fStartZ);

		if (CUBE2 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}
	void DrawCube3(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[2]);
		glTranslatef(1.0, 0.0, -3.0);
		glTranslatef(x_tran[2], 0.0, 0.0);
		glTranslatef(0.0, y_tran[2], 0.0);
		glTranslatef(0.0, 0.0, z_tran[2]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[2], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[2], 1.0, 0.0, 0.0);
		glRotatef(z_angle[2], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);
		if (CUBE3 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube4(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[3]);
		glTranslatef(-1.5, 0.0, -2.3);
		glTranslatef(x_tran[3], 0.0, 0.0);
		glTranslatef(0.0, y_tran[3], 0.0);
		glTranslatef(0.0, 0.0, z_tran[3]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[3], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[3], 1.0, 0.0, 0.0);
		glRotatef(z_angle[3], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE4 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube5(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[4]);
		glTranslatef(2.5, 0.0, -2.0);
		glTranslatef(x_tran[4], 0.0, 0.0);
		glTranslatef(0.0, y_tran[4], 0.0);
		glTranslatef(0.0, 0.0, z_tran[4]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[4], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[4], 1.0, 0.0, 0.0);
		glRotatef(z_angle[4], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE5 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube6(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[5]);
		glTranslatef(-1.0, 0.0, 2.0);
		glTranslatef(x_tran[5], 0.0, 0.0);
		glTranslatef(0.0, y_tran[5], 0.0);
		glTranslatef(0.0, 0.0, z_tran[5]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[5], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[5], 1.0, 0.0, 0.0);
		glRotatef(z_angle[5], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE6 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube7(float fStartX, float fStartY, float fStartZ) {

		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[6]);
		glTranslatef(-3.0, 0.0, 2.0);
		glTranslatef(x_tran[6], 0.0, 0.0);
		glTranslatef(0.0, y_tran[6], 0.0);
		glTranslatef(0.0, 0.0, z_tran[6]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[6], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[6], 1.0, 0.0, 0.0);
		glRotatef(z_angle[6], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE7 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube8(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[7]);
		glTranslatef(3.0, 0.0, 0.0);
		glTranslatef(x_tran[7], 0.0, 0.0);
		glTranslatef(0.0, y_tran[7], 0.0);
		glTranslatef(0.0, 0.0, z_tran[7]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[7], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[7], 1.0, 0.0, 0.0);
		glRotatef(z_angle[7], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE8 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube9(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[1]);
		glTranslatef(-4.5, 0.0, 0.3);
		glTranslatef(x_tran[8], 0.0, 0.0);
		glTranslatef(0.0, y_tran[8], 0.0);
		glTranslatef(0.0, 0.0, z_tran[8]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[8], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[8], 1.0, 0.0, 0.0);
		glRotatef(z_angle[8], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE9 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube10(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[5]);
		glTranslatef(-4.5, 0.0, -2.3);
		glTranslatef(x_tran[9], 0.0, 0.0);
		glTranslatef(0.0, y_tran[9], 0.0);
		glTranslatef(0.0, 0.0, z_tran[9]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[9], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[9], 1.0, 0.0, 0.0);
		glRotatef(z_angle[9], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE10 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube11(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[6]);
		glTranslatef(-3.0, 0.0, 0.0);
		glTranslatef(x_tran[10], 0.0, 0.0);
		glTranslatef(0.0, y_tran[10], 0.0);
		glTranslatef(0.0, 0.0, z_tran[10]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[10], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[10], 1.0, 0.0, 0.0);
		glRotatef(z_angle[10], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE11 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube12(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[3]);
		glTranslatef(4.0, 0.0, -4.6);
		glTranslatef(x_tran[11], 0.0, 0.0);
		glTranslatef(0.0, y_tran[11], 0.0);
		glTranslatef(0.0, 0.0, z_tran[11]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[11], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[11], 1.0, 0.0, 0.0);
		glRotatef(z_angle[11], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE12 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube13(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[6]);
		glTranslatef(3.0, 0.0, 1.3);
		glTranslatef(x_tran[12], 0.0, 0.0);
		glTranslatef(0.0, y_tran[12], 0.0);
		glTranslatef(0.0, 0.0, z_tran[12]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[12], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[12], 1.0, 0.0, 0.0);
		glRotatef(z_angle[12], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE13 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube14(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[7]);
		glTranslatef(-3.5, 0.0, -4.0);
		glTranslatef(x_tran[13], 0.0, 0.0);
		glTranslatef(0.0, y_tran[13], 0.0);
		glTranslatef(0.0, 0.0, z_tran[13]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[13], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[13], 1.0, 0.0, 0.0);
		glRotatef(z_angle[13], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);
		if (CUBE14 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube15(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[2]);
		glTranslatef(1.5, 0.0, 2.3);
		glTranslatef(x_tran[14], 0.0, 0.0);
		glTranslatef(0.0, y_tran[14], 0.0);
		glTranslatef(0.0, 0.0, z_tran[14]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[14], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[14], 1.0, 0.0, 0.0);
		glRotatef(z_angle[14], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);
		if (CUBE15 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void DrawCube16(float fStartX, float fStartY, float fStartZ) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_names[6]);
		glTranslatef(0.5, 0.0, 1.5);
		glTranslatef(x_tran[15], 0.0, 0.0);
		glTranslatef(0.0, y_tran[15], 0.0);
		glTranslatef(0.0, 0.0, z_tran[15]);
		shadow(fStartX, fStartY, fStartZ);
		glRotatef(y_angle[15], 0.0, 1.0, 0.0);   //y축 회전 설정
		glRotatef(x_angle[15], 1.0, 0.0, 0.0);
		glRotatef(z_angle[15], 0.0, 0.0, 1.0);

		myflat(fStartX, fStartY, fStartZ);

		if (CUBE16 == g_nSelectedObject) { DrawCubeLine(fStartX, fStartY, fStartZ); }
		glPopMatrix();
	}

	void light_read() {//light_material.txt파일을 읽어 속성값을 대입해주는 함수
		FILE* light;
		char buffer[10];

		light = fopen("light_material_gold.txt", "r");//재질 :: 골드
		//light = fopen("light_material_silver.txt", "r");// 광원의 위치가 다름 재질 :: 실버
		//light = fopen("light_material_bf.txt", "r");//조명 색 :: 포인트라이트 레드 // 디렉셔널 화이트 // 스팟라이트 초록
		//light = fopen("light_material_ruby.txt", "r");// 재질 :: 루비
		//light = fopen("light_material_emerald.txt", "r");// 재질 :: 에메랄드
		
		
		if (light == NULL) {//파일이 존재하지 않을 시
			printf("Read error : light_material.txt is not exist");//에러메시지 출력
			return;
		}
		while (!feof(light)) {//파일에 내용을 전부 반복할 때까지 반복문 실행
			fscanf(light, "%s", buffer);//내용을 버퍼에 담음

			if (strcmp(buffer, "point") == 0) {//버퍼내용이 point와 같다면 light0의 속성 값들을 지정
				fscanf_s(light, "%f %f %f", &light0_pos[0], &light0_pos[1], &light0_pos[2]);
				fscanf_s(light, "%f %f %f", &light0_ambient[0], &light0_ambient[1], &light0_ambient[2]);
				fscanf_s(light, "%f %f %f", &light0_diffuse[0], &light0_diffuse[1], &light0_diffuse[2]);
				fscanf_s(light, "%f %f %f", &light0_specular[0], &light0_specular[1], &light0_specular[2]);
			}
			else if (strcmp(buffer, "dir") == 0) {//버퍼내용이 dir와 같다면 light1의 속성 값들을 지정
				fscanf_s(light, "%f %f %f", &light1_dir[0], &light1_dir[1], &light1_dir[2]);
				fscanf_s(light, "%f %f %f", &light1_ambient[0], &light1_ambient[1], &light1_ambient[2]);
				fscanf_s(light, "%f %f %f", &light1_diffuse[0], &light1_diffuse[1], &light1_diffuse[2]);
				fscanf_s(light, "%f %f %f", &light1_specular[0], &light1_specular[1], &light1_specular[2]);
			}
			else if (strcmp(buffer, "spot") == 0) {//버퍼내용이 spot와 같다면 light2의 속성 값들을 지정
				fscanf_s(light, "%f %f %f", &light2_pos[0], &light2_pos[1], &light2_pos[2]);
				fscanf_s(light, "%f %f %f", &light2_dir[0], &light2_dir[1], &light2_dir[2]);
				fscanf_s(light, "%f", &light2_cutoff[0]);
				fscanf_s(light, "%f %f %f", &light2_ambient[0], &light2_ambient[1], &light2_ambient[2]);
				fscanf_s(light, "%f %f %f", &light2_diffuse[0], &light2_diffuse[1], &light2_diffuse[2]);
				fscanf_s(light, "%f %f %f", &light2_specular[0], &light2_specular[1], &light2_specular[2]);
			}

			else if (strcmp(buffer, "mat") == 0) {//버퍼내용이 mat와 같다면 matrial의 속성 값들을 지정
				fscanf_s(light, "%f %f %f %f", &mat_ambient[0], &mat_ambient[1], &mat_ambient[2], &mat_ambient[3]);
				fscanf_s(light, "%f %f %f %f", &mat_diffuse[0], &mat_diffuse[1], &mat_diffuse[2], &mat_diffuse[3]);
				fscanf_s(light, "%f %f %f %f", &mat_specular[0], &mat_specular[1], &mat_specular[2], &mat_specular[3]);
				fscanf_s(light, "%f", &mat_shininess[0]);
			}
		}
		fclose(light);//파일 닫기
	}

	void set_light() {//light_matrial.txt파일에서 읽어온 속성 이용해
					  //LIGHT0,1,2와 matrial을 설정 해주는 함수
					  //LIGHT0 point light
		glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
		//LIGHT1 directional light
		glLightfv(GL_LIGHT1, GL_POSITION, light1_dir);
		glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
		//LIGHT2 spotlight
		glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_dir);
		glLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, light2_cutoff);
		glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
		glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
		//glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2.0);
		//material Property
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		//glMaterialfv(GL_FRONT, GL_SHININESS, mss);
	}

	void polygon_normal_vector() {//다각형(삼각형)의 법선 벡터 계산 함수
		float x1, y1, z1; //삼각형의 첫번째 점
		float x2, y2, z2; //삼각형의  두번째 점
		float x3, y3, z3; //삼각형의  세번째 점
		float v_x1, v_y1, v_z1; // vector1
		float v_x2, v_y2, v_z2; // vector2
		float v_length;// vector lenghth
		float pnv_x = 0.0, pnv_y = 0, pnv_z = 0; //polygon normal vector

												 //삼각형의 각점들을 변수에 담아 외적을 통해 법선백터를 계산
		for (int i = 0; i < f_index; i++) {
			//법선 백터를 계산을 하기 위해 삼각형의 각 점들의 좌표를 반복문으로 받아옴
			x1 = vertex[fmt[i][0]][0];
			y1 = vertex[fmt[i][0]][1];
			z1 = vertex[fmt[i][0]][2];

			x2 = vertex[fmt[i][1]][0];
			y2 = vertex[fmt[i][1]][1];
			z2 = vertex[fmt[i][1]][2];

			x3 = vertex[fmt[i][2]][0];
			y3 = vertex[fmt[i][2]][1];
			z3 = vertex[fmt[i][2]][2];

			//삼각형의 두번째점 - 첫번째점을 계산해 백터 v1에 대입
			v_x1 = x2 - x1;
			v_y1 = y2 - y1;
			v_z1 = z2 - z1;
			//삼각형의 세번째점 - 첫번째점을 계산해 백터 v2에 대입
			v_x2 = x3 - x1;
			v_y2 = y3 - y1;
			v_z2 = z3 - z1;

			//외적 계산
			pnv_x = v_y1*v_z2 - v_y2*v_z1;
			pnv_y = v_z1*v_x2 - v_z2*v_x1;
			pnv_z = v_x1*v_y2 - v_x2*v_y1;
			v_length = sqrt(pnv_x*pnv_x + pnv_y*pnv_y + pnv_z* pnv_z);

			//삼각형의 법선백터 값을 pnv배열에 대입
			pnv[i][0] = pnv_x / v_length;
			pnv[i][1] = pnv_y / v_length;
			pnv[i][2] = pnv_z / v_length;
		}
	}

	void vertex_normal_vector2() {//각 vertex의 법선 벡터 계산 함수
		float x1, y1, z1; // 삼각형의 첫번째 점
		float x2, y2, z2; // 삼각형의 두번째 점
		float x3, y3, z3; // 삼각형의 세번째 점
		float x4, y4, z4; // 삼각형의 세번째 점
		float v_x1, v_y1, v_z1; // v1
		float v_x2, v_y2, v_z2; //v2
		float v_length;// vector length
		float vnv_x = 0.0, vnv_y = 0.0, vnv_z = 0.0;//vertex normal vector
		for (int i = 0; i < f_index2; i++) {
			//법선 백터를 계산을 하기 위해 삼각형의 각 점들의 좌표를 반복문으로 받아옴
			x1 = vertex2[fmt2[i][0]][0];
			x2 = vertex2[fmt2[i][1]][0];
			x3 = vertex2[fmt2[i][2]][0];
			x4 = vertex2[fmt2[i][3]][0];

			y1 = vertex2[fmt2[i][0]][1];
			y2 = vertex2[fmt2[i][1]][1];
			y3 = vertex2[fmt2[i][2]][1];
			y4 = vertex2[fmt2[i][3]][1];

			z1 = vertex2[fmt2[i][0]][2];
			z2 = vertex2[fmt2[i][1]][2];
			z3 = vertex2[fmt2[i][2]][2];
			z4 = vertex2[fmt2[i][3]][2];

			//삼각형의 두번째점 - 첫번째점을 계산해 백터 v1에 대입
			v_x1 = x3 - x2;
			v_y1 = y3 - y2;
			v_z1 = z3 - z2;
			//삼각형의 세번째점 - 첫번째점을 계산해 백터 v2에 대입
			v_x2 = x4 - x2;
			v_y2 = y4 - y2;
			v_z2 = z4 - z2;

			v_length = sqrt(pow(v_y1*v_z2 - v_z1*v_y2, 2) + pow(v_z1*v_x2 - v_x1*v_z2, 2) + pow(v_x1*v_y2 - v_y1*v_x2, 2));

			vnv_x = v_y1*v_z2 - v_z1*v_y2 / v_length;
			vnv_y = v_z1*v_x2 - v_x1*v_z2 / v_length;
			vnv_z = v_x1*v_y2 - v_y1*v_x2 / v_length;

			//각 점에 대한 법선 백터 대입
			vnv2[fmt2[i][0]][0] += vnv_x;
			vnv2[fmt2[i][0]][1] += vnv_y;
			vnv2[fmt2[i][0]][2] += vnv_z;
			vnv2[fmt2[i][0]][3] += 1.0;

			vnv2[fmt2[i][1]][0] += vnv_x;
			vnv2[fmt2[i][1]][1] += vnv_y;
			vnv2[fmt2[i][1]][2] += vnv_z;
			vnv2[fmt2[i][1]][3] += 1.0;

			vnv2[fmt2[i][2]][0] += vnv_x;
			vnv2[fmt2[i][2]][1] += vnv_y;
			vnv2[fmt2[i][2]][2] += vnv_z;
			vnv2[fmt2[i][2]][3] += 1.0;

			vnv2[fmt2[i][3]][0] += vnv_x;
			vnv2[fmt2[i][3]][1] += vnv_y;
			vnv2[fmt2[i][3]][2] += vnv_z;
			vnv2[fmt2[i][3]][3] += 1.0;
		}
	}

};

Cube cb[2] = {
	Cube(),
	Cube()
};
///////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 500);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("CG_Term_Project");
	glClearColor(0.0, 0.0, 0.0, 1.0);
	cb[0].obj_read();
	cb[0].obj_read2();
	cb[0].SetupTexture();
	cb[0].light_read();

	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(MyKeyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.5, 1.5, -1, 1, 1, 15);
	gluLookAt(x_ctran, 3, 3, x_ctran, 0, 0, 0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glLoadIdentity();
	glPushMatrix();

	glScalef(2.0, 1.0, 1.0);
	cb[0].myflat2();

	glPopMatrix();
	glPushMatrix();

	cb[0].Draw(GL_RENDER);
	glPopMatrix();
	glPopMatrix();

	cb[0].set_light();
	glEnable(GL_LIGHTING);
	//조건문에 따라 빛을 켜고 끔
	if (cb[0].point_type == 1)
		glEnable(GL_LIGHT0);
	else if (cb[0].point_type == 0)
		glDisable(GL_LIGHT0);
	if (cb[0].dir_type == 1)
		glEnable(GL_LIGHT1);
	else if (cb[0].dir_type == 0)
		glDisable(GL_LIGHT1);
	if (cb[0].spot_type == 1)
		glEnable(GL_LIGHT2);
	else if (cb[0].spot_type == 0)
		glDisable(GL_LIGHT2);

	glutPostRedisplay();
	glColor3f(1.0, 1.0, 1.0);

	glutSwapBuffers();
}

void MyKeyboard(unsigned char key, int x, int y) {//키보드 이벤트 처리함수
	switch (key) {
	case 'A': //Y축 +회전
		cb[0].y_angle[g_nSelectedObject - 102] += 8.0;
		break;
	case 'D': //Y축 -회전
		cb[0].y_angle[g_nSelectedObject - 102] -= 8.0;
		break;
	case 'W': //X축 +회전
		cb[0].x_angle[g_nSelectedObject - 102] += 8.0;
		break;
	case 'S': //X축 -회전
		cb[0].x_angle[g_nSelectedObject - 102] -= 8.0;
		break;
	case'Q': //z축 +회전
		cb[0].z_angle[g_nSelectedObject - 102] += 8.0;
		break;
	case'E': //z축 -회전
		cb[0].z_angle[g_nSelectedObject - 102] -= 8.0;
		break;
	case 'd':
		cb[0].x_tran[g_nSelectedObject - 102] += 0.4;
		break;
	case 'a':
		cb[0].x_tran[g_nSelectedObject - 102] -= 0.4;
		break;
	case 's':
		cb[0].z_tran[g_nSelectedObject - 102] += 0.4;
		break;
	case 'w':
		cb[0].z_tran[g_nSelectedObject - 102] -= 0.4;
		break;

	case'1': //GL_LIGHT0
		cb[0].point_type = 1 - cb[0].point_type;
		break;
	case'2': //GL_LIGHT1
		cb[0].dir_type = 1 - cb[0].dir_type;
		break;
	case'3': //GL_LIGHT2
		cb[0].spot_type = 1 - cb[0].spot_type;
		break;

	case 'z':
		if (cb[0].view_type == 0) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glFrustum(-1.5, 1.5, -1, 1, 1, 15);
			x_ctran -= 0.2;
			gluLookAt(x_ctran, 3, 3, x_ctran, 0, 0, 0, 1, 0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		break;
	case 'x':
		if (cb[0].view_type == 0) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum(-1.5, 1.5, -1, 1, 1, 15);
			x_ctran += 0.2;
			gluLookAt(x_ctran, 3, 3, x_ctran, 0, 0, 0, 1, 0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		break;

	case 27:
		exit(0);
		break;
	}
	glutPostRedisplay();//키보드 이벤트이후 그림을 다시 그려준다
}


void Mouse(int iButton, int iState, int x, int y)
{
	if (iState == GLUT_DOWN)
	{
		switch (iButton)
		{
		case GLUT_LEFT_BUTTON:
			Picking(x, y);
			break;
		}
	}
	else if (iState == GLUT_UP)
	{
		switch (iButton)
		{
		case GLUT_MIDDLE_BUTTON:
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			cb[0].view_type = 1 - cb[0].view_type;

			if (cb[0].view_type == 1) {
				glOrtho(-5.15, 5.15, -5.15, 5.15, -5, 10);//고공에서 보는 좌표
				gluLookAt(0, 2, 1, 0, 0, 0, 0, 1, 0);

			}
			else if (cb[0].view_type == 0) {
				glFrustum(-1.5, 1.5, -1, 1, 1, 15);
				gluLookAt(x_ctran, 3, 3, x_ctran, 0, 0, 0, 1, 0);
			}
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			break;
		}

	}
	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Picking(int x, int y)
{
	static unsigned int aSelectBuffer[SELECT_BUF_SIZE];
	static unsigned int uiHits;
	static int aViewport[4];

	glGetIntegerv(GL_VIEWPORT, aViewport);

	glSelectBuffer(SELECT_BUF_SIZE, aSelectBuffer);
	glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((double)x, (double)(aViewport[3] - y), 5.0, 5.0, aViewport);
	if (cb[0].view_type == 1) {
		glOrtho(-5.15, 5.15, -5.15, 5.15, -5, 10);//고공에서 보는 좌표
		gluLookAt(0, 2, 1, 0, 0, 0, 0, 1, 0);

	}
	else if (cb[0].view_type == 0) {
		glFrustum(-1.5, 1.5, -1, 1, 1, 15);
		gluLookAt(x_ctran, 3, 3, x_ctran, 0, 0, 0, 1, 0);
	}
	glMatrixMode(GL_MODELVIEW);
	// Draw!
	cb[0].Draw(GL_SELECT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	uiHits = glRenderMode(GL_RENDER);
	ProcessHits(uiHits, aSelectBuffer);

	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void ProcessHits(unsigned int uiHits, unsigned int *pBuffer)
{
	static unsigned int i, j;
	static unsigned int uiName, *ptr;

	ptr = pBuffer;
	for (i = 0; i < uiHits; i++)         // for each hit
	{
		uiName = *ptr;
		ptr += 3;

		for (j = 0; j < uiName; j++)      // for each name
		{
			switch (*ptr)
			{
			case CUBE2:
				g_nSelectedObject = CUBE2;
				printf("[ CUBE2 ]\n");
				break;
			case CUBE1:
				g_nSelectedObject = CUBE1;
				printf("[ CUBE1 ]\n");
				break;
			case CUBE3:
				g_nSelectedObject = CUBE3;
				printf("[ CUBE3 ]\n");
				break;

			case CUBE4:
				g_nSelectedObject = CUBE4;
				printf("[ CUBE4 ]\n");
				break;
			case CUBE5:
				g_nSelectedObject = CUBE5;
				printf("[ CUBE5 ]\n");
				break;
			case CUBE6:
				g_nSelectedObject = CUBE6;
				printf("[ CUBE6 ]\n");
				break;
			case CUBE7:
				g_nSelectedObject = CUBE7;
				printf("[ CUBE7 ]\n");
				break;
			case CUBE8:
				g_nSelectedObject = CUBE8;
				printf("[ CUBE8 ]\n");
				break;
			case CUBE9:
				g_nSelectedObject = CUBE9;
				printf("[ CUBE9 ]\n");
				break;
			case CUBE10:
				g_nSelectedObject = CUBE10;
				printf("[ CUBE10 ]\n");
				break;
			case CUBE11:
				g_nSelectedObject = CUBE11;
				printf("[ CUBE11 ]\n");
				break;
			case CUBE12:
				g_nSelectedObject = CUBE12;
				printf("[ CUBE12 ]\n");
				break;
			case CUBE13:
				g_nSelectedObject = CUBE13;
				printf("[ CUBE13 ]\n");
				break;
			case CUBE14:
				g_nSelectedObject = CUBE14;
				printf("[ CUBE14 ]\n");
				break;
			case CUBE15:
				g_nSelectedObject = CUBE15;
				printf("[ CUBE15 ]\n");
				break;
			case CUBE16:
				g_nSelectedObject = CUBE16;
				printf("[ CUBE16 ]\n");
				break;
			}
			ptr++;
		}
	}
}