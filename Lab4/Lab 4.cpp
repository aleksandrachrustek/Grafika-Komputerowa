#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <cmath>
#include <vector>
#include <iostream>

#define M_PI 3.141592653589793238
using namespace std;

typedef float point3[3];	// typ danych reprezentuj�cy tr�jwymiarowy punkt
int kolor;
static GLfloat viewer[] = { 0.0, 0.0, 10.0 }; // inicjalizacja po�o�enia obserwatora
static GLfloat theta = 0.0;   // k�t obrotu obiektu
static GLfloat theta2 = 0.0;  // k�t obrotu obiektu
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLint status = 0;      // stan klawiszy myszy:
							  // 0 - nie naci�ni�to �adnego klawisza
							  // 1 - naci�ni�ty zosta� lewy klawisz
							  // 2 - naci�niety zosta� prawy klawisz
static int x_pos_old = 0;     // poprzednia pozycja kursora myszy
static int y_pos_old = 0;     // poprzednia pozycja kursora myszy
static int z_pos_old = 0;     // poprzednia pozycja kursora myszy
static int delta_x = 0;       // r�nica pomi�dzy pozycj� bie��c� i poprzedni� kursora myszy
static int delta_y = 0;       // r�nica pomi�dzy pozycj� bie��c� i poprzedni� kursora myszy
static int delta_z = 0;       // r�nica pomi�dzy pozycj� bie��c� i poprzedni� kursora myszy


// Funkcja rysuj�ca osie uk�adu wsp�rz�dnych
void Axes(void)
{
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz�tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz�tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz�tek i koniec obrazu osi z

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

// Funkcja "bada" stan myszy i ustawia warto�ci odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		y_pos_old = y;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej

		status = 1;            // wci�ni�ty zosta� lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		z_pos_old = y;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		status = 2;            // wci�ni�ty zosta� prawy klawisz myszy
	}

	else
		status = 0;            // nie zosta� wci�ni�ty �aden klawisz
}

// Funkcja "monitoruje" po�o�enie kursora myszy i ustawia warto�ci odpowiednich zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;     // obliczenie r�nicy po�o�enia kursora myszy
	delta_y = y - y_pos_old;     // obliczenie r�nicy po�o�enia kursora myszy

	if (status == 1)
	{
		theta += delta_x * pix2angle;  // Modyfikacja k�ta obrotu o k�t proporcjonalny dla x do r�nicy po�o�enia kursora myszy
		theta2 += delta_y * pix2angle; // Modyfikacja k�ta obrotu o k�t proporcjonalny dla y do r�nicy po�o�enia kursora myszy
	}
	double pomocnicza = y - z_pos_old; // Zmienna pomocnicza do ustalania r�nicy po�o�enia kursora myszy przy przybli�aniu 
	if (status == 2 && viewer[2] + pomocnicza < 360 && viewer[2] + pomocnicza > 0)
	{
		viewer[2] += pomocnicza;
		x_pos_old = x; // Ustawienie bie��cego po�o�enia jako poprzednie
		y_pos_old = y; // Ustawienie bie��cego po�o�enia jako poprzednie
		z_pos_old = y; // Ustawienie bie��cego po�o�enia jako poprzednie
	}

	glutPostRedisplay();     // przerysowanie obrazu sceny
}


// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana gdy trzeba przerysowa� scen� )
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	glLoadIdentity();
	// Czyszczenie macierzy bie��cej - zamienienie jej na macierz jednostkow�
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Zdefiniowanie po�o�enia obserwatora
	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy�ej

	if (kolor == 1) glColor3f(0.0f, 1.0f, 0.0f);
	else if (kolor == 2) glColor3f(1.0f, 0.0f, 0.0f);
	else if (kolor == 3) glColor3f(0.0f, 0.0f, 1.0f);
	// Ustawienie koloru na wybrany przez u�ytkownika

	if (status == 1)                     // je�li lewy klawisz myszy wci�ni�ty
	{
		theta += delta_x * pix2angle;    // modyfikacja k�ta obrotu o kat proporcjonalny do r�nicy po�o�e� kursora myszy
	}                                     

	glRotatef(theta, 0.0, 1.0, 0.0);	//obr�t obiektu o nowy k�t
	glRotatef(theta2, 1.0, 0.0, 0.0);	//obr�t obiektu o nowy k�t

	glutWireTeapot(3.0);
	// Narysowanie czajnika

	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania
	glutSwapBuffers();
	// Zamiana przedniego bufora (gdzie aktualnie s� przechowywane dane) na bufor tylny - zawarto�� bufora tylnego b�dzie niezdefiniowana
}

// Funkcja ustalaj�ca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcz�cy (wype�nienia okna) ustawiono na czarny, czwarty agrument to przezroczystosc
}

// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal (wysoko�� i szeroko�� okna) s� przekazywane do funkcji za ka�dym razem, gdy zmieni si� rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	glMatrixMode(GL_PROJECTION);
	// Prze��czenie macierzy bie��cej na macierz projekcji
	glLoadIdentity();
	// Czyszczenie macierzy bie��cej - zamienienie jej na macierz jednostkow�           
	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametr�w dla rzutu perspektywicznego
	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci
	// relacji pomi�dzy wysoko�ci� i szeroko�ci� okna
	glMatrixMode(GL_MODELVIEW);
	// Prze��czenie macierzy bie��cej na macierz widoku modelu                                   
	glLoadIdentity();
	// Czyszczenie macierzy bie��cej - zamienienie jej na macierz jednostkow�
}

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
int main(void)
{
	cout << "Podaj kolor czajnika:\n 1.zielony, \n 2.czerwony,\n 3.niebieski: " << endl;
	cin >> kolor;
	if (kolor != 1 && kolor != 2 && kolor != 3) {
		cout << "Podano nieprawidlowy kolor! Kolor ustawiony na niebieski";
		kolor = 3;
	}
	cout << "Poruszanie kursorem myszy obraca czajnik: aby obrocic w plaszczyznie poziomej nalezy wcisnac lewy klawisz myszy i poruszac w prawo/lewo, aby obrocic w plaszczyznie pionowej nalezy wcisnac lewy klawisz myszy i poruszac w gore/dol. Aby przyblizyc nalezy wcisnac prawy klawisz myszy i poruszac w gore/dol." << endl;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Rzutowanie perspektywiczne");

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback function). 
	// Bedzie ona wywo�ywana za ka�dym razem, gdy zajdzie potrzeba przerysowania okna. 

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za zmiany rozmiaru okna.      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie inicjalizacje konieczne przed przyst�pieniem do renderowania.
	glEnable(GL_DEPTH_TEST);
	// W��czenie mechanizmu usuwania powierzchni niewidocznych
	glutMouseFunc(Mouse);
	// Ustala funkcj� zwrotn� odpowiedzialn� za badanie stanu myszy
	glutMotionFunc(Motion);
	// Ustala funkcj� zwrotn� odpowiedzialn� za badanie ruchu myszy
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}