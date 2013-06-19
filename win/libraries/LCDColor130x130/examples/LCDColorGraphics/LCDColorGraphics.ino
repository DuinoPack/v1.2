/*

LCDGraphics class demo program
Author: Federico Lanza
ingfedericolanza [at] gmail [.] com

Apr, 2012

*/

#include "LCDGraphics.h"

#include "sansserif.h"
#include "consola.h"
#include "arial.h"
#include "arialitalic.h"
#include "wingdings.h"

#define WAIT_INTERVAL    7000

// Contruct Graph object, with WHITE as Foreground colour and Black in the Background
LCDGraphics Graph(WHITE, BLACK);
unsigned long Time;

void WaitAndClear ()
{
  Graph.Foreground = WHITE;
  Graph.Background = BLACK;
  delay(WAIT_INTERVAL);
  Graph.Clear();
}

void ShowHeader (char *Str)
{
  Graph.GotoXY(1, 1);
  Graph.println(Str);
  Graph.DrawLine(1, 13, 130, 13);
  Graph.println();
}

void Welcome ()
{
  // Show welcome message for a few seconds, then clears screen
  Graph.GotoXY(40, 50);
  Graph.print("Welcome!");
  Graph.GotoXY(20, 62);
  Graph.print("Demo LCDGraphics");
  Graph.Foreground = RED;
  Graph.DrawLine(10, 74, 120, 74);
  WaitAndClear();
}

void ShowPrint ()
{
  ShowHeader("print & println");
  Graph.print("A print... "); Graph.println("...continues here");
  Graph.println();
  Graph.println("Previous line is empty");
  Graph.println("...from a println");
  WaitAndClear();
}

void ShowStringColors ()
{
  ShowHeader("Text in colors");
  Graph.println("White Text");
  Graph.Foreground = RED;  
  Graph.println("Red Text");
  Graph.Foreground = BLUE;
  Graph.println("Blue Text");
  Graph.Foreground = BLACK;
  Graph.Background = WHITE;
  Graph.println("Inverted Text");
  Graph.Foreground = RED;
  Graph.Background = BLUE;
  Graph.println("Other Combinations!");
  WaitAndClear();
}

void ShowPrints ()
{
  ShowHeader("Different data types");
  Graph.print("String: "); Graph.println("Hello World!");
  Graph.print("Uint: "); Graph.println(10);
  Graph.print("Int: "); Graph.println(-2569);
  Graph.print("Long: "); Graph.println(1234567890);
  Graph.print("Float: "); Graph.println(10.236, 3);
  WaitAndClear();
}

void ShowBases ()
{
  ShowHeader("Different bases...");
  Graph.print("DEC: "); Graph.println(1234);
  Graph.print("OCT: "); Graph.println(1234, OCT);
  Graph.print("BIN: "); Graph.println(1234, BIN);
  Graph.print("HEX: "); Graph.println(1234, HEX);
  WaitAndClear();
}

void ShowPixels ()
{
  ShowHeader("Pixels");

  Graph.PutPixel(10, 40, RED);
  Graph.PutPixel(20, 40, BLUE);
  Graph.PutPixel(30, 40, GREEN);
  Graph.PutPixel(40, 40, WHITE);
  Graph.PutPixel(50, 40, MAGENTA);
  
  Graph.PutPixel(80, 60, RED);
  Graph.PutPixel(90, 60, BLUE);
  Graph.PutPixel(100, 60, GREEN);
  Graph.PutPixel(110, 60, WHITE);
  Graph.PutPixel(120, 60, MAGENTA);
  
  Graph.PutPixel(10, 80, MAGENTA);
  Graph.PutPixel(20, 80, WHITE);
  Graph.PutPixel(30, 80, GREEN);
  Graph.PutPixel(40, 80, BLUE);
  Graph.PutPixel(50, 80, RED);

  Graph.PutPixel(80, 100, MAGENTA);
  Graph.PutPixel(90, 100, WHITE);
  Graph.PutPixel(100, 100, GREEN);
  Graph.PutPixel(110, 100, BLUE);
  Graph.PutPixel(120, 100, RED);

  WaitAndClear();
}

void ShowLines ()
{
  ShowHeader("Lines");

  Graph.Foreground = RED;
  Graph.DrawLine(5, 18, 65, 125);
  Graph.Foreground = BLUE;
  Graph.DrawLine(62, 20, 125, 125);
  Graph.Foreground = YELLOW;
  Graph.DrawLine(30, 15, 82, 125);
  Graph.Foreground = WHITE;
  Graph.DrawLine(2, 93, 100, 30);
  Graph.Foreground = GREEN;
  Graph.DrawLine(115, 38, 17, 93);
  
  WaitAndClear();
}

void ShowRectangles ()
{
  ShowHeader("Squares");
  
  Graph.Foreground = WHITE;
  Graph.DrawRectangle(8, 38, 28, 58);
  Graph.Foreground = RED;
  Graph.DrawRectangle(20, 40, 100, 110);
  Graph.Foreground = BLUE;
  Graph.DrawRectangle(15, 90, 115, 118);
  
  WaitAndClear();
}

void ShowCircles ()
{
  ShowHeader("Circles");

  Graph.Foreground = BLUE;
  Graph.DrawCircle(65, 80, 40);
  Graph.Foreground = RED;
  Graph.DrawCircle(33, 60, 25);
  Graph.Foreground = GREEN;
  Graph.DrawCircle(80, 60, 40);
  
  WaitAndClear();
}

void Bye ()
{
  // Show welcome message for a few seconds, then clears screen
  Graph.GotoXY(35, 50);
  Graph.print("See you soon!");
  Graph.Foreground = RED;
  Graph.DrawLine(10, 74, 120, 74);
  WaitAndClear();
}


void setup ()
{
  // Any of these fonts are available
  // replace the following line with any of the commented out that follows the and you´ll see the demo
  // in that particular font
    Graph.SetFont(SansSerif_Charmap, SansSerif_Descriptors, SANSSERIF_HEIGHT);
  //  Graph.SetFont(Consola_Charmap, Consola_Descriptors, CONSOLA_HEIGHT);
  //  Graph.SetFont(Arial_Charmap, Arial_Descriptors, ARIAL_HEIGHT);
  //  Graph.SetFont(ArialItalic_Charmap, ArialItalic_Descriptors, ARIALITALIC_HEIGHT);
  //  Graph.SetFont(Wingdings_Charmap, Wingdings_Descriptors, WINGDINGS_HEIGHT);
}

void loop ()
{
  // Welcome message
  Welcome();
  // Show print capabilities
  ShowPrint();
  // Strings and Colors
  ShowStringColors();
  // Show different print methods
  ShowPrints();
  // Show different bases for integers
  ShowBases();
  // Pixels
  ShowPixels();
  // Lines
  ShowLines();
  // Rectangles
  ShowRectangles();
  // Circles
  ShowCircles();
  //
  Bye();
}
