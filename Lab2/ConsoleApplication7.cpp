#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

float size = 2;
float squaresize = size * 2; //Polepszenie widoczno�ci

float SquareColour()
{
	float x = rand() % 100;
	return x / 99;
}

float Perturbation()
{
	float x = rand() % 500;
	return x / 499;
}

void SmallSquare(float x, float y)
{
	float _x = x - 3 * squaresize;
	float _y = y + 3 * squaresize;
	int a = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (a != 4)
			{
				float p = Perturbation();
				glBegin(GL_POLYGON);
				glColor3f(SquareColour(), SquareColour(), SquareColour()); //Losowanie koloru dla pierwszego rogu
				glVertex2f(_x + Perturbation(), _y + Perturbation());
				glColor3f(SquareColour(), SquareColour(), SquareColour()); //Losowanie koloru dla drugiego rogu
				glVertex2f(_x + Perturbation() + squaresize, _y + Perturbation());
				glColor3f(SquareColour(), SquareColour(), SquareColour()); //Losowanie koloru dla trzeciego rogu
				glVertex2f(_x + Perturbation() + squaresize, _y + Perturbation() - squaresize);
				glColor3f(SquareColour(), SquareColour(), SquareColour()); //Losowanie koloru dla czwartego rogu
				glVertex2f(_x + Perturbation(), _y + Perturbation() - squaresize);
				glEnd();
				glFlush();
				a++;
			}
			else a++;
			_x = _x + 1 * squaresize;
		}
		_x = _x - 3 * squaresize;
		_y = _y - 1 * squaresize;
	}
}

void MiddleSquare(float x, float y)
{
	int a = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (a != 4)
			{
				SmallSquare(x, y);
				a++;
			}
			else a++;
			x = x + 3 * squaresize;
		}
		x = x - 9 * squaresize;
		y = y - 3 * squaresize;
	}
}

void BigSquare(float x, float y)
{
	int a = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (a != 4)
			{
				MiddleSquare(x- 10* squaresize,y+ 10* squaresize);
				a++;
			}
			else a++;
			x = x + 9 * squaresize;
		}
		x = x - 27 * squaresize;
		y = y - 9 * squaresize;
	}
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	BigSquare(0, 0);
}

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor okna wn�trza okna - ustawiono na czarny
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szeroko�� i wysoko�� okna) s�
// przekazywane do funkcji za ka�dym razem, gdy zmieni si� rozmiar okna
{
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio okre�laj�cej proporcj� wymiar�w okna

	if (vertical == 0)
		// Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielko�ci okna urz�dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION);
	// Okre�lenie uk�adu wsp�rz�dnych obserwatora

	glLoadIdentity();
	// Okre�lenie przestrzeni ograniczaj�cej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wsp�czynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// okre�lenie okna obserwatora.
	// Pozwala to zachowa� w�a�ciwe proporcje rysowanego obiektu
	// Do okre�lenia okna obserwatora s�u�y funkcja glOrtho(...)

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
	else
		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Okre�lenie uk�adu wsp�rz�dnych    

	glLoadIdentity();
}

int main()
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wy�wietlania
	// GLUT_SINGLE - pojedynczy bufor wy�wietlania
	// GLUT_RGBA - model kolor�w RGB

	glutCreateWindow("Dywan Sierpi�skiego");
	// Utworzenie okna i okre�lenie tre�ci napisu w nag��wku okna

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback)
	// Biblioteka GLUT b�dzie wywo�ywa�a t� funkcj� za ka�dym razem, gdy
	// trzeba b�dzie przerysowa� okno

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powy�ej) wykonuje wszelkie 
	// inicjalizacje konieczne przed przyst�pieniem do renderowania

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

	return 0;
}