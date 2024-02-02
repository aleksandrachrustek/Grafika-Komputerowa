#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <cmath>
#include <iostream>

using namespace std;

typedef float point3[3];
const int N = 100;
GLfloat light_positionSecond[] = { -30.0, 0.0, -10.0, 1.0 }; // położenie źródła
GLfloat light_position[] = { 30.0, 0.0, -10.0, 1.0 };		 // położenie źródła
static GLfloat angle[] = { 0.0, 0.0, 0.0 };
static GLfloat viewer[] = { 0.0, 0.0, 10.0 }; // inicjalizacja polozenia obserwatora
static GLfloat theta = 0.0;     // kat obrotu obiektu
static GLfloat theta2 = 0.0;    // kat obrotu obiektu
static GLfloat pix2angle;       // przelicznik pikseli na stopnie
int zoomIT = 0;
int zoomItOld = 0;
static int zoom = 30;

static GLint status = 0;      // stan klawiszy myszy
// 0 - nie nacisnieto zadnego klawisza
// 1 - nacisniety zostal lewy klawisz
// 2 - nacisniety zostal prawy klawisz

static int x_pos_old = 0;        // poprzednia pozycja kursora myszy
static int y_pos_old = 0;

static int x_pos_old2 = 0;       // poprzednia pozycja kursora myszy
static int y_pos_old2 = 0;

static int fi_pos_old = 0;
static int fi_pos_old2 = 0;

static int theta_pos_old = 0;
static int theta_pos_old2 = 0;

static int delta_x = 0;       // roznica pomiedzy pozycja biezaca i poprzednia kursora myszy
static int delta_y = 0;

static int delta_fi = 0;
static int delta_fi2 = 0;
static int delta_theta = 0;
static int delta_theta2 = 0;

double R = 13;
float fi = 0.0;
float fi2 = 0.0;

#define M_PI 3.14159265358979323846


// Funkcja rysujaca osie ukladu wspolrzednych
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

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania osi - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);

	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania osi - niebieski
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
		status = 1;			  // wcisniety zostal lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 2;
	}
	else
	{
		status = 0;          // nie zostal wcisniety zaden klawisz
	}
}


// Funkcja "monitoruje" polozenie kursora myszy i ustawia wartosci odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old; // Obliczenie roznicy polozenia kursora myszy
	delta_y = y - y_pos_old; // Obliczenie roznicy polozenia kursora myszy
	x_pos_old = x;
	y_pos_old = y;

	glutPostRedisplay(); // Przerysowanie obrazu sceny
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

	//zamiana dziedziny parametrycznej na postac 3D wedlug okreslonego wzoru
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			u = (float)i / (N - 1);
			v = (float)j / (N - 1);

			tab[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v); // X(u,v)
			tab[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5; //Y(u,v)
			tab[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v); //Z(u,v)

			float ux, uy, uz, vx, vy, vz, length;

			// obliczenie wartosci wektorow normalnych wedlug wzorow z instrukcji
			ux = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
			uy = (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
			uz = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);

			vx = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
			vy = 0;
			vz = -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);

			vectorNorm[i][j][0] = uy * vz - uz * vy; // opisanie wektorow normalnych wedlug instrukcji
			vectorNorm[i][j][1] = uz * vx - ux * vz;
			vectorNorm[i][j][2] = ux * vy - uy * vx;

			length = sqrt(vectorNorm[i][j][0] * vectorNorm[i][j][0] + vectorNorm[i][j][1] * vectorNorm[i][j][1] + vectorNorm[i][j][2] * vectorNorm[i][j][2]);
			// obliczenie dlugosci wektorow - konieczne do normalizacji wektorow

			if (i < N / 2)  //jesli jestesmy w pierwszej polowie jajka
			{
				vectorNorm[i][j][0] = (uy * vz - uz * vy) / length;
				vectorNorm[i][j][1] = (uz * vx - ux * vz) / length;
				vectorNorm[i][j][2] = (ux * vy - uy * vx) / length;
			}
			else if (i > N / 2) //jesli jestesmy w drugiej polowie jajka
			{
				vectorNorm[i][j][0] = -1 * (uy * vz - uz * vy) / length;
				vectorNorm[i][j][1] = -1 * (uz * vx - ux * vz) / length;
				vectorNorm[i][j][2] = -1 * (ux * vy - uy * vx) / length;
			}
			else if (i == 0 || i == N)
			{
				vectorNorm[i][j][0] = 0;
				vectorNorm[i][j][1] = -1;
				vectorNorm[i][j][2] = 0;
			}
			else
			{
				vectorNorm[i][j][0] = 0;
				vectorNorm[i][j][1] = 1;
				vectorNorm[i][j][2] = 0;
			}
		}
	}

	glColor3f(1.0f, 1.0f, 1.0f);  // Ustawienie koloru jajka na bialy

	// Rysowanie siatki trojkatow 
	int i = 0;
	for (i = 0; i < N - 1;)
	{
		for (int j = 0; j < N - 1; j++)
		{
			glBegin(GL_TRIANGLES);

			glNormal3fv(vectorNorm[i][j + 1]);
			glVertex3fv(tab[i][j + 1]);

			glNormal3fv(vectorNorm[i + 1][j]);
			glVertex3fv(tab[i + 1][j]);

			glNormal3fv(vectorNorm[i + 1][j + 1]);
			glVertex3fv(tab[i + 1][j + 1]);

			glNormal3fv(vectorNorm[i + 1][j]);
			glVertex3fv(tab[i + 1][j]);

			glNormal3fv(vectorNorm[i][j + 1]);
			glVertex3fv(tab[i][j + 1]);

			glNormal3fv(vectorNorm[i][j]);
			glVertex3fv(tab[i][j]);

			glEnd();

		}
		i++;
	}
}


// Funkcja okreslajaca co ma byc rysowane (zawsze wywolywana gdy trzeba
// przerysowac scene)
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczacym
	glLoadIdentity();
	// Czyszczenie macierzy biezacej
	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wyzej
	gluLookAt(0, 0, viewer[2], 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);

	if (status == 1) {
		theta += delta_x * pix2angle;     // modyfikacja kąta obrotu o kat proporcjonalny
		fi += delta_y * pix2angle;
	}

	if (status == 2) {
		theta2 += delta_x * pix2angle;    // modyfikacja kąta obrotu o kat proporcjonalny
		fi2 += delta_y * pix2angle;
	}

	float pi = 3.14;
	if (fi > 2 * pi) fi = 2 * pi;		  // Dziedzina dla pi i fi
	if (theta > 2 * pi) theta = 2 * pi;
	if (fi < 0) fi = 0;
	if (theta < 0) theta = 0;

	if (fi2 > 2 * pi) fi2 = 2 * pi;
	if (theta2 > 2 * pi) theta2 = 2 * pi;
	if (fi2 < 0) fi2 = 0;
	if (theta2 < 0) theta2 = 0;

	//Ustalenie położenia źródła swiatła pierwszego według instrukcji
	light_position[0] = zoom * cos(theta2) * cos(fi2);
	light_position[1] = zoom * sin(fi2);
	light_position[2] = zoom * sin(theta2) * cos(fi2);

	//Ustalenie położenia źródła swiatła drugiego według instrukcji
	light_positionSecond[0] = zoom * cos(theta) * cos(fi);
	light_positionSecond[1] = zoom * sin(fi);
	light_positionSecond[2] = zoom * sin(theta) * cos(fi);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light_positionSecond);

	Egg();

	glFlush();
	// Przekazanie polecen rysujacych do wykonania

	glutSwapBuffers();
	// Zamienienie bufora przedniego na tylny

}

// Funkcja ustalajaca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczacy (wypelnienia okna) ustawiono na czarny


	// Definicja materiału z jakiego zrobione jest jajko
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspolczynniki ka =[kar,kag,kab] dla swiatla otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspolczynniki kd =[kdr,kdg,kdb] swiatla rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspolczynniki ks =[ksr,ksg,ksb] dla swiatla odbitego               

	GLfloat mat_shininess = { 60.0 };
	// wspolczynnik n opisujacy polysk powierzchni


	// Definicja zrodla swiatla
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
	// polozenie zrodla

	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	// skladowe intensywnosci swiecenia zrodla swiatla otoczenia Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
	// skladowe intensywnosci swiecenia zrodla swiatla powodujacego odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// skladowe intensywnosci swiecenia zrodla swiatla powodujacego odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant = { 1.0 };
	// skladowa stala ds dla modelu zmian oswietlenia w funkcji odleglosci od zrodla

	GLfloat att_linear = { 0.05 };
	// skladowa liniowa dl dla modelu zmian oswietlenia w funkcji odleglosci od zrodla

	GLfloat att_quadratic = { 0.001 };
	// skladowa kwadratowa dq dla modelu zmian oswietlenia w funkcji odleglosci od zrodla

	// drugie zrodlo swiatla
	GLfloat light_position2[] = { 0.0, 0.0, 0.0,1.0 };

	GLfloat light_ambient2[] = { 0.1, 0.1, 0.1, 1.0 };	
	// składowe intensywności świecenia źródła światła otoczenia Ia = [Iar,Iag,Iab]
	GLfloat light_diffuse2[] = { 1.0, 0.0,0.0, 10.0 }; 
	// składowe intensywności świecenia źródła światła powodującego odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat light_specular2[] = { 1.0, 0.0,1.0, 10.0 };	
	// składowe intensywności świecenia źródła światła powodującego dbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat att_constant2 = { 1.0 }; 
	// składowa stała ds dla modelu zmian oświetlenia w funkcji odległości od źródła
	GLfloat att_linear2 = { (GLfloat)0.05 }; 
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji odległości od źródła
	GLfloat att_quadratic2 = { (GLfloat)0.001 }; 
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji odległości od źródła

	// Ustawienie parametrow materialu
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// Ustawienie parametrow zrodla
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant2);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear2);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic2);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

	// Ustawienie opcji systemu oswietlania sceny
	glShadeModel(GL_SMOOTH); // wlaczenie lagodnego cieniowania
	glEnable(GL_LIGHTING);   // wlaczenie systemu oswietlenia sceny
	glEnable(GL_LIGHT0);     // wlaczenie zrodla o numerze 0
	glEnable(GL_LIGHT1);     // włączenie źródła o numerze 1
	glEnable(GL_DEPTH_TEST); // wlaczenie mechanizmu z bufora

}


// Funkcja ma za zadanie utrzymanie stalych proporcji rysowanych w przypadku zmiany rozmiarow okna.
// Parametry vertical i horizontal (wysokosc i szerokosc okna) sa przekazywane do funkcji za kazdym razem gdy zmieni sie rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;
	pix2angle *= 0.009;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	GLdouble aspect = (GLdouble)horizontal / vertical;

	gluPerspective(70, (float)aspect, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkości okna okna widoku (viewport) w zależności relacji pomiędzy wysokością i szerokością okna
	glMatrixMode(GL_MODELVIEW);
	// Przelaczenie macierzy biezacej na macierz widoku modelu                                    
	glLoadIdentity();
	// Czyszczenie macierzy biezacej - zastapienie jej macierza jednostkowa
}


void main(void)
{
	cout << "Aby zmienic miejsce pierwszego zrodla swiatla nalezy wcisnac lewy przycisk myszy i poruszac kursorem, Aby zmienic miejsce drugiego zrodla swiatla nalezy wcisnac prawy przycisk myszy i poruszac kursorem." << endl;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(900, 900);

	glutCreateWindow("OpenGL - oświetlanie scen 3-D");

	glutDisplayFunc(RenderScene);
	// Okreslenie, ze funkcja RenderScene bedzie funkcja zwrotna (callback function).
	// Bedzie ona wywolywana za kazdym razem, gdy zajdzie potrzeba przerysowania okna.

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcje zwrotna odpowiedzialna za zmiany rozmiaru okna      

	MyInit();
	// Funkcja MyInit() (zdefiniowana powyzej) wykonuje wszelkie
	// inicjalizacje konieczne przed przystapieniem do renderowania 

	glEnable(GL_DEPTH_TEST);
	// Wylaczenie przeliczania dla powierzchni niewidocznych

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light_positionSecond);

	glutMouseFunc(Mouse);
	// Ustala funkcje zwrotna odpowiedzialna za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcje zwrotna odpowiedzialna za badanie ruchu myszy

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}