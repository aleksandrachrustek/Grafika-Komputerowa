#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>
#include <conio.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

typedef float point2[2];
point2** textureCord;
GLfloat light_position[] = { 30.0, 0.0, -10.0, 1.0 }; // położenie źródła
typedef float point3[3];
const int N = 80;
static GLfloat angle[] = { 0.0, 0.0, 0.0 };
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
static GLfloat theta = 0.0;   // kat obrotu obiektu
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static int zoom = 30;

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
#pragma pack(1)            
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}
	TGAHEADER;
#pragma pack(8)

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;

	// Wartosci domyslne zwracane w przypadku bledu
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;

	// Przeczytanie naglówka pliku
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	// Odczytanie szerokosci, wysokosci i glebi obrazu
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

	// Sprawdzenie, czy glebia spelnia zalozone warunki (8, 24, lub 32 bity)
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	// Obliczenie rozmiaru bufora w pamieci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	// Alokacja pamieci dla danych obrazu
	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}

	// Ustawienie formatu OpenGL
	switch (sDepth)
	{
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};

	fclose(pFile);

	return pbitsperpixel;
}
/*************************************************************************************/

static GLint status = 0;      // stan klawiszy
							  // 0 - nie nacisnieto zadnego klawisza
							  // 1 - nacisniety zostal lewy klawisz
							  // 2 - nacisniety prawy klawisz
static int x_pos_old = 0;     // poprzednia pozycja kursora myszy
static int y_pos_old = 0;
static int fi_pos_old = 0;
static int theta_pos_old = 0;
static int delta_x = 0;       // róznica pomiedzy pozycja biezaca i poprzednia kursora myszy
static int delta_y = 0;
static int delta_fi = 0;
static int delta_theta = 0;
double R = 13;
float fi = 0.0;
float fi2 = 0.0;
#define M_PI 3.14159265358979323846


void spinEgg()
{
	angle[0] -= 0.5;
	if (angle[0] > 360.0) angle[0] -= 360.0;

	angle[1] -= 0.5;
	if (angle[1] > 360.0) angle[1] -= 360.0;

	angle[2] -= 0.5;
	if (angle[2] > 360.0) angle[2] -= 360.0;

	glutPostRedisplay(); // odswiezenie zawartosci aktualnego okna
}

// Funkcja rysujaca osie ukladu wspólrzednych
void Axes(void)
{
	point3  x_min = { -50.0, 0.0, 0.0 };
	point3  x_max = { 50.0, 0.0, 0.0 };
	// poczatek i koniec obrazu osi x

	point3  y_min = { 0.0, -100.0, 0.0 };
	point3  y_max = { 0.0, 50.0, 0.0 };
	// poczatek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -50.0 };
	point3  z_max = { 0.0, 0.0, 50.0 };
	//  poczatek i koniec obrazu osi z

	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

// Funkcja "monitoruje" polozenie kursora myszy i ustawia wartosci odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;    // obliczenie róznicy polozenia kursora myszy
	x_pos_old = x;              // podstawienie biezacego polozenia jako poprzednie


	delta_y = y - y_pos_old;    // obliczenie róznicy polozenia kursora myszy
	y_pos_old = y;              // podstawienie biezacego polozenia jako poprzednie

	glutPostRedisplay();        //przerysowanie obrazu sceny
}

void Egg() {
	float tab[100][100][3];
	float u = 0.0;
	float v = 0.0;
	point3** vectors3d = new point3 * [N];
	point3** vectorNorm = new point3 * [N];
	for (int i = 0; i < N; i++) {
		vectors3d[i] = new point3[N];
		vectorNorm[i] = new point3[N];
	}
	//zamiana dziedziny parametrycznej na postac 3-D wedlug okreslonego wzoru
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			u = (float)i / (N - 1);
			v = (float)j / (N - 1);

			tab[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v); // X(u,v)
			tab[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5; //Y(u,v)
			tab[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v); //Z(u,v)
			float ux, uz, uy, vz, vy, vx, length;

			// obliczenie wartosci wektorow normalnych
			ux = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(3.14 * v);
			uy = (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
			uz = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(3.14 * v);

			vx = 3.14 * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(3.14 * v);
			vy = 0;
			vz = -3.14 * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(3.14 * v);
			textureCord[i][j][0] = v;    //wspolrzedne tekstury
			textureCord[i][j][1] = u;
			vectorNorm[i][j][0] = uy * vz - uz * vy;
			vectorNorm[i][j][1] = uz * vx - ux * vz;
			vectorNorm[i][j][2] = ux * vy - uy * vx;

			length = sqrt(vectorNorm[i][j][0] * vectorNorm[i][j][0] + vectorNorm[i][j][1] * vectorNorm[i][j][1] + vectorNorm[i][j][2] * vectorNorm[i][j][2]);
			if (i == 0 || i == N)
			{
				vectorNorm[i][j][0] = 0;
				vectorNorm[i][j][1] = -1;
				vectorNorm[i][j][2] = 0;
			}
			else if (i < N / 2)
			{
				vectorNorm[i][j][0] = (uy * vz - uz * vy) / length;
				vectorNorm[i][j][1] = (uz * vx - ux * vz) / length;
				vectorNorm[i][j][2] = (ux * vy - uy * vx) / length;
			}
			else if (i > N / 2)
			{
				vectorNorm[i][j][0] = -1 * (uy * vz - uz * vy) / length;
				vectorNorm[i][j][1] = -1 * (uz * vx - ux * vz) / length;
				vectorNorm[i][j][2] = -1 * (ux * vy - uy * vx) / length;
			}
			else
			{
				vectorNorm[i][j][0] = 0;
				vectorNorm[i][j][1] = 1;
				vectorNorm[i][j][2] = 0;
			}
		}
	}
	int i = 0;
	for (i = 0; i < N - 1;)
	{
		for (int j = 0; j < N - 1; j++)
		{
			glColor3f(255.0f, 255.0f, 255.0f);
			glBegin(GL_TRIANGLES);

			glNormal3fv(vectorNorm[i][j + 1]);
			glTexCoord2f(textureCord[i][j + 1][0], textureCord[i][j + 1][1]);
			glVertex3fv(tab[i][j + 1]);

			glNormal3fv(vectorNorm[i + 1][j]);
			glTexCoord2f(textureCord[i + 1][j][0], textureCord[i + 1][j][1]);
			glVertex3fv(tab[i + 1][j]);

			glNormal3fv(vectorNorm[i + 1][j + 1]);
			glTexCoord2f(textureCord[i + 1][j + 1][0], textureCord[i + 1][j + 1][1]);
			glVertex3fv(tab[i + 1][j + 1]);

			glNormal3fv(vectorNorm[i + 1][j]);
			glTexCoord2f(textureCord[i + 1][j][0], textureCord[i + 1][j][1]);
			glVertex3fv(tab[i + 1][j]);

			glNormal3fv(vectorNorm[i][j + 1]);
			glTexCoord2f(textureCord[i][j + 1][0], textureCord[i][j + 1][1]);
			glVertex3fv(tab[i][j + 1]);

			glNormal3fv(vectorNorm[i][j]);
			glTexCoord2f(textureCord[i][j][0], textureCord[i][j][1]);
			glVertex3fv(tab[i][j]);

			glEnd();
		}
		i++;
	}
}
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, viewer[2], 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
	light_position[0] = zoom * cos(theta) * cos(fi2);
	light_position[1] = zoom * sin(fi2);
	light_position[2] = zoom * sin(theta) * cos(fi2);
	glRotatef(angle[0], 1.0, 0.0, 0.0);
	glRotatef(angle[1], 0.0, 1.0, 0.0);
	glRotatef(angle[2], 0.0, 0.0, 1.0);
	Egg();
	glutSwapBuffers();
}

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GLbyte* pBytes;
	GLint ImWidth;
	GLint ImHeight;
	GLint ImComponents;
	GLenum ImFormat;
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess = { 100.0 };
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat att_constant = { 1.0 };
	GLdouble att_linear = { 0.05 };
	GLdouble att_quadratic = { 0.001 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	pBytes = LoadTGAImage("P3_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);		// ładowanie z pliku tekstury
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;
	pix2angle *= 0.009;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)horizontal / vertical;
	gluPerspective(70, (float)aspect, 1.0, 30.0);
	glViewport(0, 0, horizontal, vertical);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void textureAlocation()							// Alokacja tablicy do tekstur
{
	textureCord = new point2 * [N + 1];
	for (int i = 0; i < N + 1; i++)
	{
		textureCord[i] = new point2[N + 1];
	}
}
void main(void)
{
	textureAlocation();
	glutIdleFunc(spinEgg);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Teksturowanie");
	glutDisplayFunc(RenderScene);
	glutMotionFunc(Motion);
	glutReshapeFunc(ChangeSize);
	MyInit();
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}