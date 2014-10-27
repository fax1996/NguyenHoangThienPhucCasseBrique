/**************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
using namespace std;

int read_keyboard();

/*************************/

/* Creer la zone du jeu */ 

#define H 15
#define L 20
#define B 7 //longue de la barre
#define S 100 //Vitesse du jeu
#define P 100 //ratio points/touche

char screen[H][L];
int barre=L/2-2;
int score=0;

void screen_display(){
  system("clear");
  for(int i=0; i<H; i++){
    for(int j=0; j<L; j++){
      cout << screen[i][j];
    }
    cout << endl;
  }
}

void clear_screen() {
  for(int i=0; i<H; i++)
    for (int j=0; j<L; j++)
	screen[i][j]=' ';
}

void frontier() //creer le frontier du jeu
{
  system("clear");
  for(int i=0; i<H; i++)
    {
      for(int j=0; j<L; j++)
	if( i==0 || j==0 || i==H-1 || j==L-1 )
	screen[i][j]='*';
      cout << endl;
    }
}
/**********************************/
/* Creer la barre */

void barreB(int key)
{
  if (key == 'a' && barre>1) //prende "a" pour aller a la gauche
    barre -= 1;
  if(key == 'd'&& (barre+B+2)<L+1) //prendre "d" pour aller a la droite
    barre += 1;
  for(int a=0;a<B;a++)
    screen[H-2][barre+a] = '=';
}

/********************************/
/* Creer la balle */

int x=1, y=1;
bool gd=1, hb=1;

void balle()
{  
  if (gd==1) //balle: gauche a droite
    { 
      if (hb==1) //balle: haut a bas
	{
	  screen[x][y]='o'; x++; y++; 
	}
      
      if (screen[x][y]=='=' || screen[x][y-1]=='=') //toucher la barre
	{
	  hb=0; x--; y--;
	}      
   
      if (y==L-1 && hb==1) //toucher frontier a droite et aller au frontier au dessous
	{
	  gd=0; y--; x--;
	}

      if (x==H+1) //fin
	{
	  cout << "Meilleure chance a la prochaine fois" << endl;
	  cout << "Score: " << score*P << endl;
	  exit(0);
	}
      
      if (hb==0) //balle: bas a haut
	{
	  screen[x][y]='o'; x--; y++;
	}
      
      if (x==0) //toucher frontier au-dessus
	{
	  hb=1; x++;
	}
	
      if (y==L-1 && hb==0) //toucher frontier a droite et aller au frontier au dessus
	{
	  gd=0; y--; x++;
	}
    }
  
  if (gd==0) //balle: droite a gauche
    {
      if (hb==1) //balle: haut a bas
	{
	  screen[x][y]='o'; y--; x++;
	}
      
      if (screen[x][y]=='=' || screen[x][y+1]=='=') //toucher la barre
	{
	  hb=0; x--; y++;
	  screen[x][y]='o'; x--; y++;
	}
        
       
      if (y==0 && hb==1) //toucher frontier a gauche et aller au frontier au dessous
	{
	  gd=1; y++;
	}

      
      if (x==H+1) //fin
	{
	  cout << "Meilleure chance a la prochaine fois" << endl;
	  cout << "Score: " << score*P << endl;
	  exit(0);
	}

      if (hb==0) //balle: bas a haut
	{
	  screen[x][y]='o'; y--; x--;
	}

      if (x==0) //toucher frontier au dessus
	{
	  hb=1; x++;
	}
     
      if (y==0 && hb==0) //toucher frontier a gauche et aller au frontier au dessus
	{
	  gd=1; y++;
	}
    }
}
/*********************************/

/*Brique*/
int briqueL=L/2-4;
int briqueH=H/2-4;
void briqueB()
{
   for (int b; b<5; b++)
      screen[briqueH][briqueL+b] = 'x';
  
  if (gd==1 && hb==1) //balle: gauche-droite + haut-bas
    {
      if (screen[x][y] == 'x' || screen[x][y+1] == 'x')
	{
	  hb=0; x-=2;
	  srand(time(NULL));
	  briqueH = rand()%(H/2-3)+1;
	  briqueL = rand()%(L-7)+1;
	  score++;
	}
    }
  
  if (gd==1 && hb==0) //balle: gauche-droite + bas-haut
    {
      if (screen[x][y] == 'x' || screen[x][y+1] == 'x')
	{
	  hb=1; x+=2;
	  srand(time(NULL));
	  briqueH = rand()%(H/2-3)+1;
	  briqueL = rand()%(L-7)+1;
	  score++;
	}
    }
  
  if (gd==0 && hb==1) //balle: droite-gauche + haut-bas
    {
      if (screen[x][y] == 'x' || screen[x][y-1] == 'x')
	{
	  hb=0; x-=2;
	  srand(time(NULL));
	  briqueH = rand()%(H/2-3)+1;
	  briqueL = rand()%(L-7)+1;
          score++;
	}
    }
  
  if (gd==0 && hb==0) //balle: droite-gauche + bas-haut
    {
      if (screen[x][y] == 'x' || screen[x][y-1] == 'x')
	{
	  hb=1; x+=2;
	  srand(time(NULL));
	  briqueH = rand()%(H/2-3)+1;
	  briqueL = rand()%(L-7)+1;	  
	  score++;
	}
    }
}
/********************************/

/* Bouton */



int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
  
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  
  ch = getchar();
  
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
  
  if(ch != EOF)
    {
      ungetc(ch, stdin);
      return 1;
    }
  
  return 0;
} 
int read_keyboard() {
  int key;
  if (kbhit())
    key = getc(stdin);
  else
    key = 0;
  return key;
}
  
/**********************************/

int main()
{
  int key;
  do
    {
    key = read_keyboard();
    clear_screen();
    frontier();
    barreB(key);
    balle();
    briqueB();
    screen_display();
    usleep(S * 1000);
    }
  while (key != 'q');
  return(0);
}
