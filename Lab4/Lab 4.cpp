#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <cmath>
#include <vector>
#include <iostream>

#define M_PI 3.141592653589793238
using namespace std;

typedef float point3[3];	// typ danych reprezentuj¹cy trójwymiarowy punkt
int kolor;
static GLfloat viewer[] = { 0.0, 0.0, 10.0 }; // inicjalizacja po³o¿enia obserwatora
static GLfloat theta = 0.0;   // k¹t obrotu obiektu
static GLfloat theta2 = 0.0;  // k¹t obrotu obiektu
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLint status = 0;      // stan klawiszy myszy:
							  // 0 - nie naciœniêto ¿adnego klawisza
							  // 1 - naciœniêty zosta³ lewy klawisz
							  // 2 - naciœniety zosta³ prawy klawisz
static int x_pos_old = 0;     // poprzednia pozycja kursora myszy
static int y_pos_old = 0;     // poprzednia pozycja kursora myszy
static int z_pos_old = 0;     // poprzednia pozycja kursora myszy
static int delta_x = 0;       // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹ i poprzedni¹ kursora myszy
static int delta_y = 0;       // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹ i poprzedni¹ kursora myszy
static int delta_z = 0;       // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹ i poprzedni¹ kursora myszy


// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(void)
{
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz¹tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz¹tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz¹tek i koniec obrazu osi z

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

// Funkcja "bada" stan myszy i ustawia wartoœci odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		y_pos_old = y;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej

		status = 1;            // wciœniêty zosta³ lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		z_pos_old = y;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		status = 2;            // wciœniêty zosta³ prawy klawisz myszy
	}

	else
		status = 0;            // nie zosta³ wciœniêty ¿aden klawisz
}

// Funkcja "monitoruje" po³o¿enie kursora myszy i ustawia wartoœci odpowiednich zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy
	delta_y = y - y_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy

	if (status == 1)
	{
		theta += delta_x * pix2angle;  // Modyfikacja k¹ta obrotu o k¹t proporcjonalny dla x do ró¿nicy po³o¿enia kursora myszy
		theta2 += delta_y * pix2angle; // Modyfikacja k¹ta obrotu o k¹t proporcjonalny dla y do ró¿nicy po³o¿enia kursora myszy
	}
	double pomocnicza = y - z_pos_old; // Zmienna pomocnicza do ustalania ró¿nicy po³o¿enia kursora myszy przy przybli¿aniu 
	if (status == 2 && viewer[2] + pomocnicza < 360 && viewer[2] + pomocnicza > 0)
	{
		viewer[2] += pomocnicza;
		x_pos_old = x; // Ustawienie bie¿¹cego po³o¿enia jako poprzednie
		y_pos_old = y; // Ustawienie bie¿¹cego po³o¿enia jako poprzednie
		z_pos_old = y; // Ustawienie bie¿¹cego po³o¿enia jako poprzednie
	}

	glutPostRedisplay();     // przerysowanie obrazu sceny
}


// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba przerysowaæ scenê )
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej - zamienienie jej na macierz jednostkow¹
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Zdefiniowanie po³o¿enia obserwatora
	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy¿ej

	if (kolor == 1) glColor3f(0.0f, 1.0f, 0.0f);
	else if (kolor == 2) glColor3f(1.0f, 0.0f, 0.0f);
	else if (kolor == 3) glColor3f(0.0f, 0.0f, 1.0f);
	// Ustawienie koloru na wybrany przez u¿ytkownika

	if (status == 1)                     // jeœli lewy klawisz myszy wciœniêty
	{
		theta += delta_x * pix2angle;    // modyfikacja k¹ta obrotu o kat proporcjonalny do ró¿nicy po³o¿eñ kursora myszy
	}                                     

	glRotatef(theta, 0.0, 1.0, 0.0);	//obrót obiektu o nowy k¹t
	glRotatef(theta2, 1.0, 0.0, 0.0);	//obrót obiektu o nowy k¹t

	glutWireTeapot(3.0);
	// Narysowanie czajnika

	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania
	glutSwapBuffers();
	// Zamiana przedniego bufora (gdzie aktualnie s¹ przechowywane dane) na bufor tylny - zawartoœæ bufora tylnego bêdzie niezdefiniowana
}

// Funkcja ustalaj¹ca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcz¹cy (wype³nienia okna) ustawiono na czarny, czwarty agrument to przezroczystosc
}

// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹ przekazywane do funkcji za ka¿dym razem, gdy zmieni siê rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	glMatrixMode(GL_PROJECTION);
	// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji
	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej - zamienienie jej na macierz jednostkow¹           
	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego
	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci
	// relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna
	glMatrixMode(GL_MODELVIEW);
	// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu                                   
	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej - zamienienie jej na macierz jednostkow¹
}

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli
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
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback function). 
	// Bedzie ona wywo³ywana za ka¿dym razem, gdy zajdzie potrzeba przerysowania okna. 

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za zmiany rozmiaru okna.      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie inicjalizacje konieczne przed przyst¹pieniem do renderowania.
	glEnable(GL_DEPTH_TEST);
	// W³¹czenie mechanizmu usuwania powierzchni niewidocznych
	glutMouseFunc(Mouse);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie stanu myszy
	glutMotionFunc(Motion);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie ruchu myszy
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}