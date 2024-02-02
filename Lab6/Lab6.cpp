#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>
#include <math.h>
#include <cmath>
#define _USE_MATH_DEFINES

using namespace std;

typedef float point3[3];
float PI = 3.1415926535897932384626433832795;
int model = 3;
int N = 100;
int s[5] = { 0, 0, 0, 0, 0 };						// Tablica pozwala wyswietlac kazdy z wierzchołkow
bool state_1 = false;
bool state_2 = false;
bool state_3 = false;
bool state_4 = false;
bool state_5 = false;
bool state_6 = false;
struct punkty
{
	double x;
	double y;
	double z;
};
static GLfloat viewer[] = { 0.0, 0.0, 250.0 };

// Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
// FileName, alokuje pamiec i zwraca wskaznik (pBits) do bufora w którym
// umieszczone sa dane.
// Ponadto udostêpnia szerokosdc (ImWidth), wysokosc (ImHeight) obrazu
// tekstury oraz dane opisujace format obrazu wedlug specyfikacji OpenGL
// (ImComponents) i (ImFormat).
// Jest to bardzo uproszczona wersja funkcji wczytujacej dane z pliku TGA.
// Dziala tylko dla obrazów wykorzystujacych 8, 24, or 32 bitowy kolor.
// Nie obsluguje plików w formacie TGA kodowanych z kompresja RLE.
GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
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
	}TGAHEADER;
#pragma pack(8)

	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte    *pbitsperpixel = NULL;

	// Wartosci domyslne zwracane w przypadku bledu
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;
	//_CRT_SECURE_NO_WARNINGS
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

static GLfloat theta = 0.0;   // kat obrotu obiektu
static GLfloat theta2 = 0.0;  // kat obrotu obiektu
static GLfloat pix2angle;     // przelicznik pikseli na stopnie

static GLint status = 0;      // stan klawiszy myszy
							  // 0 - nie nacisnieto zadnego klawisza
							  // 1 - nacisniety zostal lewy klawisz
							  // 2 - nacisniety zostal prawy klawisz

static int x_pos_old = 0;     // poprzednia pozycja kursora myszy
static int y_pos_old = 0;
static int z_pos_old = 0;

static int delta_x = 0;       // róznica pomiedzy pozycja biezaca i poprzednia kursora myszy
static int delta_y = 0;
static int delta_z = 0;

// Funkcja rysujaca osie ukladu wspólrzednych
void Axes(void)
{
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// poczatek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0, 5.0, 0.0 };
	// poczatek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0, 5.0 };
	// poczatek i koniec obrazu osi y

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

// Funkcja "bada" stan myszy i ustawia wartosci odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		y_pos_old = y;
		status = 1;           // wcisniety zostal lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		z_pos_old = y;
		status = 2;
	}
	else
		status = 0;         // nie zostal wcisniety zaden klawisz
}

// Funkcja "monitoruje" polozenie kursora myszy i ustawia wartosci odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;    // obliczenie róznicy polozenia kursora myszy
	delta_y = y - y_pos_old;
	double pom = y - z_pos_old;
	if (status == 2 && viewer[2] + pom < 270 && viewer[2] + pom > 30) viewer[2] += pom;

	x_pos_old = x;             // podstawienie biezacego polozenia jako poprzednie
	y_pos_old = y;
	z_pos_old = y;

	glutPostRedisplay();      // przerysowanie obrazu sceny
}

// Funkcja okreslajaca co ma byc rysowane (zawsze wywolywana, gdy trzeba przerysowac scene)
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczacym

	glLoadIdentity();
	// Czyszczenie macierzy biezacej

	gluLookAt(viewer[0], viewer[1], viewer[2] / 10, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej powyzej

	if (status == 1)                    // jesli lewy klawisz myszy wcisniety
	{
		theta += delta_x * pix2angle;   // modyfikacja kata obrotu o kat proporcjonalny do róznicy polozen kursora myszy
		theta2 += delta_y * pix2angle;
	}

	glRotatef(theta, 0.0, 1.0, 0.0);    // obrót obiektu o nowy kat
	glRotatef(theta2, 1.0, 0.0, 0.0);

	glColor3f(1.0f, 1.0f, 1.0f);

	if (s[0] != state_1)				// wciśnieta cyfra 1
	{
		glBegin(GL_TRIANGLES);			// pierwszy trójkąt
		glNormal3f(0.0f, 0.0f, 1.0f);	// Wektory normalne z poprzednich zajeć
		glTexCoord2f(0.0f, 0.0f);		// Funkcja określająca, które wierzchołki trójkąta naniesionego na wzorzec tekstury odpowiadają którym wierzchołkom
		glVertex3f(0.0f, 10.0f, 5.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-10.0f, 0.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(10.0f, 0.0f, 0.0f);
		glEnd();
	}
	if (s[1] != state_2)
	{									//drugi trójkąt
		glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 10.0f, 5.0f);
		glNormal3f(10.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, 0.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(10.0f, 0.0f, 10.0f);
		glEnd();
	}
	if (s[2] != state_3)
	{									//trzeci trójkąt
		glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 10.0f, 5.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, 0.0f, 10.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(-10.0f, 0.0f, 10.0f);
		glEnd();
	}
	if (s[3] != state_4)
	{									//czwarty trójkąt
		glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 10.0f, 5.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-10.0f, 0.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(-10.0f, 0.0f, 10.0f);
		glEnd();
	}
	if (s[4] != state_5)
	{									//piaty trójkąt
		glBegin(GL_POLYGON); 			//poligon pozwala na stworzenie podstawy piramidy
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, 0.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, 0.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(10.0f, 0.0f, 10.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-10.0f, 0.0f, 10.0f);
		glEnd();
	}

	glutSwapBuffers();
}

// Funkcja ustalajaca stan renderowania
void MyInit(void)
{
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	pBytes = LoadTGAImage("P3_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	// Zdefiniowanie tekstury 2D
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	// Zwolnienie pamieci
	free(pBytes);

	// Wlaczenie mechanizmu teksturowania
	glEnable(GL_TEXTURE_2D);

	// Ustalenie trybu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Okreslenie sposobu nakladania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczacy (wypelnienia okna) ustawiono na czarny
}


// Funkcja ma za zadanie utrzymanie stalych proporcji rysowanych w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokosc i szerokosc okna) sa przekazywane do funkcji za kazdym razem gdy zmieni sie rozmiar okna.

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal; // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);
	// Przelaczenie macierzy biezacej na macierz projekcji

	glLoadIdentity();
	// Czyszczenie macierzy biezacej

	gluPerspective(70, 1.0, 5.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego

	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkosci okna okna widoku (viewport) w zaleznosci od relacji pomiedzy wysokoscia i szerokoscia okna

	glMatrixMode(GL_MODELVIEW);
	// Przelaczenie macierzy biezacej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy biezacej- zamienienie jej na macierz jednostkowa
}

void keys(unsigned char key, int x, int y)
{
	if (key == '1') state_1 = true;
	if (key == '2') state_2 = true;
	if (key == '3') state_3 = true;
	if (key == '4') state_4 = true;
	if (key == '5') state_5 = true;
	if (key == '6')
	{
		state_1 = false;
		state_2 = false;
		state_3 = false;
		state_4 = false;
		state_5 = false;
	}
	RenderScene(); // przerysowanie obrazu sceny
}

void main(void)
{
	cout << "Przyciski:" << endl<<"1 - pierwsza sciana"<< endl << "2 - druga sciana" << endl << "3 - trzecia sciana" << endl << "4 - czwarta sciana" << endl << "5 - piata sciana" << endl << "6 - znikaja wszystkie sciany" << endl;
	cout << "Piramida mozna obracac przytrzymujac lewy przycisk myszy i przyblizac przytrzymujac prawy przycisk."<< endl;
	 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(1000, 1000);

	glutCreateWindow("Teksturowanie piramidy");

	glutDisplayFunc(RenderScene);

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcje zwrotna odpowiedzialna za zmiany rozmiaru okna                        

	MyInit();
	// Funkcja MyInit() (zdefiniowana powyzej) wykonuje wszelkie inicjalizacje konieczne przed przystapieniem do renderowania

	glEnable(GL_DEPTH_TEST);
	// Wlaczenie mechanizmu nieprzeliczania niewidocznych elementów sceny

	glutKeyboardFunc(keys);

	glutMouseFunc(Mouse);
	// Ustala funkcja zwrotna odpowiedzialna za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcje zwrotna odpowiedzialna za badanie ruchu myszy

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}
