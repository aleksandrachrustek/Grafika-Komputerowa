﻿
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <cmath>
#include <time.h>
#include <vector>
#include <iostream>

using namespace std;

typedef float point3[3];
int N;
static int model = 0;
static GLfloat angle[] = { 0.0, 0.0, 0.0 };
static float colors_tab[100][100][3];
static GLfloat viewer[] = { 0.0, 0.0, 0.0 }; // inicjalizacja polozenia obserwatora
static GLfloat theta = 0.0;					 // kat obrotu obiektu
static GLfloat theta2 = 0.0;				 // kat obrotu obiektu
static GLfloat pix2angle;					 // przelicznik pikseli na stopnie
static GLfloat pix2angle_x;					 // przelicznik pikseli na stopnie
static GLfloat pix2angle_y;					 // przelicznik pikseli na stopnie

static GLint status = 0;	    // stan klawiszy myszy
								// 0 - nie nacisnieto zadnego klawisza
								// 1 - nacisniety zostal lewy klawisz
								// 2 - nacisniety prawy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int y_pos_old = 0;
static int z_pos_old = 0;

static int delta_x = 0;         // różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int delta_y = 0;
static int delta_z = 0;

double R = 13;
float fi = 0.0;

#define M_PI 3.14159265358979323846


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

// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		y_pos_old = y;
		status = 1;           // wci?ni?ty zosta? lewy klawisz myszy
	}

	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		z_pos_old = y;
		status = 2;			 // wciśnięty został prawy klawisz myszy
	}

	else
		status = 0;         // nie został wciśnięty żaden klawisz
}

// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old; // obliczenie różnicy położenia kursora myszy
	delta_y = y - y_pos_old; // obliczenie różnicy położenia kursora myszy

	if (status == 1)		 // Jeśli lewy klawisz wciśnięty
	{
		theta += delta_x * pix2angle_x;		// modyfikacja kąta obrotu 
		fi += delta_y * pix2angle_y;		// do róznicy połozenia kursora myszy
		if (theta >= 360.0)					// jeśli kąt >= 360 stopni
			theta = 0.0;					// to kąt = 0
		if (fi >= 360.0)
			fi = 0.0;
	}
	else if (status == 2) {	 // Jeśli lewy klawisz wciśnięty
		R += 0.01 * delta_y; // przyblizanie się obserwatora do obiektu
		if (R <= 8.0)		 // ustalone maksymalne przyblizenia i oddalenia
			R = 8.0;		 // aby nie wejść w środek jajka
		if (R >= 13.0)
			R = 13.0;
	}
	x_pos_old = x;           // ustawienie aktualnego położenia jako poprzednie
	y_pos_old = y;
	z_pos_old = y;
	glutPostRedisplay();    // przerysowanie obrazu sceny
}

void Egg() {
	float tab[100][100][3];

	float u = 0.0;
	float v = 0.0;

	//zamiana dziedziny parametrycznej na postac 3-D wedlug okreslonego wzoru
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			u = (float)i / (N - 1);
			v = (float)j / (N - 1);

			tab[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v); // X(u,v)
			tab[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5; //Y(u,v)
			tab[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v); //Z(u,v)
		}

	//rysowanie punktów
	if (model == 0)
	{
		glBegin(GL_POINTS);
		for (int i = 0; i < N - 1; ++i) {
			for (int j = 0; j < N - 1; ++j)
			{
				glVertex3fv(tab[i][j]);
			}
		}
		glEnd();
	}

	//rysowanie linii
	if (model == 1)
	{

		for (int i = 0; i < N - 1; i++) {
			for (int j = 0; j < N - 1; j++)
			{
				glBegin(GL_LINES);
				glVertex3fv(tab[i][j]);
				glVertex3fv(tab[i][j + 1]);

				glVertex3fv(tab[i][j]);
				glVertex3fv(tab[i + 1][j]);

				glVertex3fv(tab[i + 1][j]);
				glVertex3fv(tab[i][j + 1]);

				glVertex3fv(tab[i + 1][j]);
				glVertex3fv(tab[i + 1][j + 1]);
			}
		}
		glEnd();
	}
	//rysowanie kolorowych trójkatów
	if (model == 2)
	{
		int i = 0;
		for (i = 0; i < N - 1;)
		{
			for (int j = 0; j < N - 1; j++)
			{
				glBegin(GL_TRIANGLES);

				glColor3fv(colors_tab[i][j + 1]);
				glVertex3fv(tab[i][j + 1]);

				glColor3fv(colors_tab[i + 1][j]);
				glVertex3fv(tab[i + 1][j]);

				glColor3fv(colors_tab[i + 1][j + 1]);
				glVertex3fv(tab[i + 1][j + 1]);

				glColor3fv(colors_tab[i + 1][j]);
				glVertex3fv(tab[i + 1][j]);

				glColor3fv(colors_tab[i][j + 1]);
				glVertex3fv(tab[i][j + 1]);

				glColor3fv(colors_tab[i][j]);
				glVertex3fv(tab[i][j]);

				glEnd();
			}
			i++;
		}
	}
}

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba przerysować scenę )
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczacym
	glLoadIdentity();
	// Czyszczenie macierzy biezacej- zamienienie jej na macierz jednostkową

	viewer[0] = R * cos(theta) * cos(fi);
	viewer[1] = R * sin(fi);
	viewer[2] = R * sin(theta) * cos(fi);

	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, cos(fi), 0.0);

	glRotatef(theta, 0.0, 1.0, 0.0);  //obrót obiektu o nowy kat
	glRotatef(theta2, 1.0, 0.0, 0.0);

	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wyzej
	glColor3f(0.0f, 0.7f, 0.5f);
	// Ustawienie koloru rysowania
	glRotatef(angle[0], 1.0, 0.0, 0.0);
	glRotatef(angle[1], 0.0, 1.0, 0.0);
	glRotatef(angle[2], 0.0, 0.0, 1.0);

	glRotated(45.0, 1.0, 0.0, 0.0);

	Egg();

	glFlush();
	// Przekazanie polecen rysujacych do wykonania
	glutSwapBuffers();
	// Zamiana przedniego bufora (gdzie aktualnie są przechowywane dane) na bufor tylny - zawartość bufora tylnego będzie niezdefiniowana
}

//funkcja zwrotna do wyboru rodzaju modelu, pobrana ze strony zsk
void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 0;
	if (key == 'w') model = 1;
	if (key == 's') model = 2;

	RenderScene(); // przerysowanie obrazu sceny
}

// Funkcja ustalajaca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcacy (wypelnienia okna) ustawiono na czarny
}

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle_x = 360.0 * 0.1 / (float)horizontal; // przeliczenie pikseli na stopnie
	pix2angle_y = 360.0 * 0.1 / (float)vertical;
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  okreslajacej proporcje wymiarów okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkosciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Przelaczenie macierzy biezacej na macierz projekcji 
	glLoadIdentity();
	// Czyszcznie macierzy biezacej - zamienienie jej na macierz jednostkową            

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wspólczynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest okreslenie tak zwanej
	// przestrzeni ograniczajacej pozwalajacej zachowac wlasciwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczjacej sluzy funkcja
	// glOrtho(...)            
;
	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego
	glMatrixMode(GL_MODELVIEW);
	// Przelaczenie macierzy biezacej na macierz widoku modelu                                    
	glLoadIdentity();
	// Czyszcznie macierzy biezacej - zamienienie jej na macierz jednostkową   

}

float createRandom()
{
	float randomNumber;
	double const generateRandom = rand() % 10;
	randomNumber = generateRandom / 10;

	return randomNumber;
}
void randomcolors() {
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int l = 0; l < 3; l++)
			{
				colors_tab[i][j][l] = createRandom();
			}
		}
	}
}

void main(void)
{
	cout << "Podaj liczbe n z zakresu 1 do 100: ";
	cin >> N;
	if (N <= 0 || N >= 101) {
		cout << "Podano nieprawidlowa liczbe! Liczba n zostanie ustawiona na 100";
		N = 100;
	}
	cout << "Jajko mozna obracac przy kliknietym lewym przycisku myszy i przyblizac przy kliknietym prawym."<<endl;

	randomcolors();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 800);

	glutCreateWindow("Jajko");

	glutDisplayFunc(RenderScene);
	// Okreslenie, ze funkcja RenderScene bedzie funkcja zwrotna
	// (callback function).  Bedzie ona wywolywana za kazdym razem
	// gdy zajdzie potrzba przeryswania okna 
	glutKeyboardFunc(keys);

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcje zwrotna odpowiedzialna
	// za zmiany rozmiaru okna      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powyzej) wykonuje wszelkie
	// inicjalizacje konieczne przed przystapieniem do renderowania 
	glEnable(GL_DEPTH_TEST);
	// Wlaczenie mechanizmu usuwania powierzchni niewidocznych
	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy
	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}