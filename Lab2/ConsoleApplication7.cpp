#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

float size = 2;
float squaresize = size * 2; //Polepszenie widocznoœci

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
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
	BigSquare(0, 0);
}

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor okna wnêtrza okna - ustawiono na czarny
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szerokoœæ i wysokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem, gdy zmieni siê rozmiar okna
{
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio okreœlaj¹cej proporcjê wymiarów okna

	if (vertical == 0)
		// Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœci okna urz¹dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION);
	// Okreœlenie uk³adu wspó³rzêdnych obserwatora

	glLoadIdentity();
	// Okreœlenie przestrzeni ograniczaj¹cej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wspó³czynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// okreœlenie okna obserwatora.
	// Pozwala to zachowaæ w³aœciwe proporcje rysowanego obiektu
	// Do okreœlenia okna obserwatora s³u¿y funkcja glOrtho(...)

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
	else
		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Okreœlenie uk³adu wspó³rzêdnych    

	glLoadIdentity();
}

int main()
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wyœwietlania
	// GLUT_SINGLE - pojedynczy bufor wyœwietlania
	// GLUT_RGBA - model kolorów RGB

	glutCreateWindow("Dywan Sierpiñskiego");
	// Utworzenie okna i okreœlenie treœci napisu w nag³ówku okna

	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback)
	// Biblioteka GLUT bêdzie wywo³ywa³a t¹ funkcjê za ka¿dym razem, gdy
	// trzeba bêdzie przerysowaæ okno

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powy¿ej) wykonuje wszelkie 
	// inicjalizacje konieczne przed przyst¹pieniem do renderowania

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

	return 0;
}