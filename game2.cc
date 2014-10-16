/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>
#include <fcntl.h>
using namespace std;

/* retourne 0 si aucune touche n'est appuyée, 
 * sinon retourne le code ASCII de la touche */
  int read_keybord();

/******************************************************************************/

/******************************************************************************/
/* Mettre votre code source ici */
int main(){

  int n=20;
  int key;
  bool descente = false;
  do{
    system("clear");
    for(int i=0;i<n;i++)
      cout << endl;
    cout << "hello world" << endl;
    
    if (descente)
      n=(n+1)%20;
    else {
      n=n+1;
      if(n==0) n=20;
    }
    
    key = read_keybord();
    if(key == 'z')
      descente = true;
    if(key == 'a')
      descente = false;
    
    usleep(20 * 1000);
  } while(key != 'q');
}

/******************************************************************************/

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

int read_keybord() {
  int key;
  if (kbhit())
    key = getc(stdin);
  else
    key = 0;
  return key;
}


/******************************************************************************/
/******************************************************************************/
