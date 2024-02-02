#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <cmath>
#include <vector>
#include <iostream>

#define M_PI 3.141592653589793238
using namespace std;

typedef float point3[3]; // typ danych reprezentujący trójwymiarowy punkt
int N;
int kolor;
int model = 1;
float colours[100][100][3];
static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // trzy kąty obrotu

void spinEgg()
{
	theta[0] -= 0.5;
	if (theta[0] > 360.0) theta[0] -= 360.0;

	theta[1] -= 0.5;
	if (theta[1] > 360.0) theta[1] -= 360.0;

	theta[2] -= 0.5;
	if (theta[2] > 360.0) theta[2] -= 360.0;

	glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
}

// Funkcja rysująca osie układu współrzędnych
void Axes(void)
{
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// początek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// początek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  początek i koniec obrazu osi z

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

void Egg() {

	float tab[100][100][3];
	float u = 0.0;
	float v = 0.0;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			u = (float)i / (N - 1); // Obliczenie stosunku wartości i do zakresu (N - 1), co daje wartość u w zakresie od 0.0 do 1.0
			v = (float)j / (N - 1); // Obliczenie stosunku wartości j do zakresu (N - 1), co daje wartość v w zakresie od 0.0 do 1.0

			tab[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
			//X(u,v)
			tab[i][j][1] = (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2));
			//Y(u,v)
			tab[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);
			//Z(u,v)
		}
	}

	// Rysowanie jajka za pomocą kropek
	if (model == 1) {

		glBegin(GL_POINTS);
		for (int i = 0; i < N - 1; ++i) {
			for (int j = 0; j < N - 1; ++j) {
				glVertex3fv(tab[i][j]);
			}
		}
		glEnd();
	}
	// Rysowanie jajka za pomocą linii
	if (model == 2) {

		glBegin(GL_LINES);
		for (int i = 0; i < N - 1; ++i) {
			for (int j = 0; j < N - 1; ++j) {

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
	// Rysowanie jajka za pomocą modelu złożonego z trójkątów
	if (model == 3) {

		for (int i = 0; i < N - 1; ++i) {
			for (int j = 0; j < N - 1; ++j) {

				glBegin(GL_TRIANGLES);

				glColor3fv(colours[i][j + 1]);
				glVertex3fv(tab[i][j + 1]);

				glColor3fv(colours[i + 1][j]);
				glVertex3fv(tab[i + 1][j]);

				glColor3fv(colours[i + 1][j + 1]);
				glVertex3fv(tab[i + 1][j + 1]);

				glColor3fv(colours[i + 1][j]);
				glVertex3fv(tab[i + 1][j]);

				glColor3fv(colours[i][j + 1]);
				glVertex3fv(tab[i][j + 1]);

				glColor3fv(colours[i][j]);
				glVertex3fv(tab[i][j]);

			}
		}
		glEnd();
	}
}

float makeRandomNumber() {
	float random;
	double const generateRandom = rand() % 10;

	random = generateRandom / 10;

	return random;
}

void makeRandomColour() {

	for (int i = 0; i < N - 1; ++i) {
		for (int j = 0; j < N - 1; ++j) {
			for (int k = 0; k < N - 1; ++k) {

				colours[i][j][k] = makeRandomNumber();
			}
		}
	}
}

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba przerysować scenę )
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym
	glLoadIdentity();
	// Czyszczenie macierzy bieżącej - zamienienie jej na macierz jednostkową
	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej
	glRotated(45.0, 2.0, 2.0, 2.0);
	// Obrót o 45 stopni względem punktów (0,0,0) i (2,2,2)

	if (kolor == 1) glColor3f(0.0f, 1.0f, 0.0f); 
	else if (kolor == 2) glColor3f(1.0f, 0.0f, 0.0f);
	else if (kolor == 3) glColor3f(0.0f, 0.0f, 1.0f);
	// Ustawienie koloru na wybrany przez użytkownika

	glTranslated(0.0, -5.0, 0.0);
	// Przesunięcie obiektu o 5 jednostek w dół w trójwymiarowej przestrzeni

	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);
	
	Egg();
	// Wywołanie funkcji odpowiedzialnej za rysowanie jajka
	glFlush();
	// Przekazanie poleceń rysujących do wykonania
	glutSwapBuffers();
	// Zamiana przedniego bufora (gdzie aktualnie są przechowywane dane) na bufor tylny - zawartość bufora tylnego będzie niezdefiniowana
}

// Funkcja ustalająca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcący (wypełnienia okna) ustawiono na czarny
}

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  określającej proporcje wymiarów okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkości okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji
	glLoadIdentity();
	// Czyszczenie macierzy bieżącej - zamienienie jej na macierz jednostkową           
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie współczynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej przestrzeni ograniczającej pozwalającej zachować właściwe
	// proporcje rysowanego obiektu. Do określenia przestrzeni ograniczającej służy funkcja glOrtho(...).           
	if (horizontal <= vertical)
		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else
		glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu                                   
	glLoadIdentity();
	// Czyszczenie macierzy bieżącej - zamienienie jej na macierz jednostkową
}

void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1; // chmura punktów
	if (key == 'w') model = 2; // model w postaci siatki
	if (key == 's') model = 3; // model złożony z wypełnionych trójkątów 

	RenderScene(); // przerysowanie obrazu sceny
}

// Główny punkt wejścia programu. Program działa w trybie konsoli
int main(void)
{
	cout << "Podaj liczbe n z zakresu 1 do 100: ";
	cin >> N;
	if (N <= 0 || N >= 101) {
		cout << "Podano nieprawidlowa liczbe! Liczba n zostanie ustawiona na 100";
		N = 100;
	}
	cout << "Podaj kolor jajka:\n 1.zielony, \n 2.czerwony,\n 3.niebieski: "<<endl;
	cin >> kolor;
	if (kolor != 1 && kolor != 2 && kolor != 3) {
		cout << "Podano nieprawidlowy kolor! Kolor ustawiony na niebieski";
		kolor = 3;
	}

	makeRandomColour();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Układ współrzędnych 3D");

	glutIdleFunc(spinEgg);
	// Funkcja odpowiedzialna za obrót jajka
	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback function). 
	// Bedzie ona wywoływana za każdym razem, gdy zajdzie potrzeba przerysowania okna. 
	glutKeyboardFunc(keys);
	// Wywołanie funkcji keys pozwalającej na przełączanie przy użyciu klawiszy klawiatury.
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za zmiany rozmiaru okna.      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie inicjalizacje konieczne przed przystąpieniem do renderowania.
	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania powierzchni niewidocznych
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}