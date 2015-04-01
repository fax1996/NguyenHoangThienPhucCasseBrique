/*************************/
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
//~ On va definir certaines couleurs ici
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Noir */
#define RED     "\033[31m"      /* Rouge */
#define GREEN   "\033[32m"      /* Vert */
#define YELLOW  "\033[33m"      /* Jaune */
#define BLUE    "\033[34m"      /* Bleu */
#define MAGENTA "\033[35m"      /* Violet */

#define H 16  //~ hauteur de la zone du jeu
#define L 29  //~ longueur de la zone du jeu
#define B 11  //~ longueur de la raquette
/*************************/
//~ Certaines fonctions pour supplementer les autres
void play();
void replay();
void introduction();
/*************************/
//~ Parametres + Valeurs
char screen[H][L]; int level=1; int score;
bool start=false;
int x=H-3, y=L/2;
int barre=L/2-6;
int hb=0,gd; 

/**************--LE CORPS DU JEU--***********/
//~ Creer la zone du jeu
void frontier(){
	for (int i=0; i<H; i++)
		for (int j=0; j<L; j++)
			if (i==0||j==0||i==H-1||j==L-1) screen[i][j]='*';
}

//~ Apparaitre le contenu et les informations du jeu
void screen_display(){ 
	system("clear");
	switch(level){
		case 1 :{ cout << RED << "AVANCEZ!!!" << RESET << endl; break; } 
		case 2 :{ cout << YELLOW << "PARFAIT!!!" << RESET << endl; break; } 
		case 3 :{ cout << MAGENTA << "EXCELLENT!!!" << RESET << endl; break; } 
	}
	cout << "* NIVEAU " << level;
	cout << " | Scores: " << score*100 << endl; //~ Pour chaque brique detruite, on gagne 100 points
	
	for (int i=0;i<H;i++){ 
		for (int j=0; j<L; j++)
			cout << screen[i][j];
		cout << endl;
    }
}

//~ Apparaitre les briques
void brique_display(){
	if(level==1)
		for (int i=1;i<4;i++)         
			for (int j=5; j<L-5; j++){ 
				if (i%2!=0)
					screen[i][j]='x';   //~ On a 2 lignes de briques pour le niveau 1...
			}
	if(level==2 || level==3)
		for (int i=1;i<6;i++)         
			for (int j=5; j<L-5; j++){ 
				if (i%2!=0)
					screen[i][j]='x';	//~ et 3 lignes pour le reste!
			}
}

void clear_screen(){ 
	for (int i=1; i<H-1; i++)
		for (int j=1; j<L-1; j++)
			if ((screen[i][j]=='o' && start==false) || screen[i][j]=='x')
				continue;
			else
				screen[i][j] = ' ';
}

/***********--QUELQUES CHOSES POUR FINIR LE JEU--**************/
//~ Si l'on a rate le jeu...
void finish_lose(){
	int key;
	cout << RED << "\nMeilleure chance a la prochaine fois" << RESET << endl;
	cout << " | Scores: " << YELLOW << score*100 << RESET << endl;
	cout << "Rejouer? [y/n]";
	do{
		key = read_keyboard();
		if(key == 'y') replay();
		if(key == 'n') 	exit(0);
	}while (key != 'n');
}
//~ et dans la situation de la victoire!
void finish_win(){
	int key;
	cout << YELLOW << "\nCONGRATULATIONS!!!" << RESET << endl;
	cout << " | Scores: " << YELLOW << score*100 << RESET << endl;
	cout << "Rejouer? [y/n]";
	do{
		key = read_keyboard();
		if(key == 'y') replay();
		if(key == 'n') 	exit(0);
	}while (key != 'n');
}

/*************--CREER LA RAQUETTE--************/
void barreB(int key){ 
	if (key == 'a' && barre>1) 	//~ On prend [a] pour deplacer a la gauche
		barre -= 1;
    if (key == 'd' && (barre+B+2)<L)
		barre += 1;				//~ et [d] pour la droit
	int a;
    for(a=0;a<B;a++)
		screen[H-2][barre+a+1] = '='; //~ La raquette est creee en construire les meme elements apres le premier.
	if (start == false)  
		screen[H-3][L/2]='o'; //~ Juste apres quand on prenez le [JOUER], la balle a une
							  //~ position fixe au milieu et au-dessous du frontier du jeu
}

/************--LE DEPLACEMENT DE LA BALLE--*************/
void update_game(int key){
	if (start==true){ //~ On commence le jeu!
		if (gd==1){ 		//~ Balle: gauche a droite
			if (hb==1){ 	//~ Balle: haut en bas
				screen[x][y]='o'; x++; y++;
				if (screen[x][y]=='x'){ hb=0; screen[x-1][y-1]=' '; score++;
					if (screen[x][y+1]=='x') score++;
					if (screen[x][y-1]=='x') score++;} //~ si la balle touche la brique et apres sa, si a cote de la balle
													   //~ les deux positions) les autres briques existent, on va gagner 
													   //~ tous les points. 
				if (screen[x][y]==' ' && screen[x][y+1]=='x') score++;
				if (screen[x][y]==' ' && screen[x][y-1]=='x') score++;
				//~ dans la situation si la balle touche pas la brique mais
				//~ un ( deux ) cote(s) il existe les briques.
			}
			if (screen[x][y]== '=' || screen[x][y-1]== '='){ hb=0; x--; y--; }
			if (y==L-1 && hb==1){ gd=0;	x--; y--; } 
			if (y==L-1 && hb==0){ gd=0; x++; y--; }
			
			if (x==H) finish_lose();
			
			if (hb==0){ 	//~ Balle: bas en haut
				screen[x][y]='o'; x--; y++;
				if (screen[x][y]=='x'){ hb=1; score++;
					if (screen[x][y+1]=='x') score++;
					if (screen[x][y-1]=='x') score++;}
				if (screen[x][y]==' ' && screen[x][y+1]=='x') score++;
				if (screen[x][y]==' ' && screen[x][y-1]=='x') score++;
			}
			if (x==0){ hb=1; x+=2; }	
		}
      
		if (gd==0){ 		//~ Balle: droite a gauche
			if (hb==1){ 	//~ Balle: haut en bas
				screen[x][y]='o'; x++; y--;
				if (screen[x][y]=='x'){ hb=0; screen[x-1][y+1]=' '; score++; 
					if (screen[x][y+1]=='x') score++;
					if (screen[x][y-1]=='x') score++;}
				if (screen[x][y]==' ' && screen[x][y+1]=='x') score++;
				if (screen[x][y]==' ' && screen[x][y-1]=='x') score++;
			}
			if (screen[x][y]== '=' || screen[x][y+1]== '='){ hb=0; x--; y++; }
			if (y==0) { gd=1; y+=2; } 
				
			if (x==H) finish_lose();
			
			if (hb==0){ 	//~ Balle: bas en haut
				screen[x][y]='o'; x--; y--;
				if (screen[x][y]=='x') { hb=1;	score++;
					if (screen[x][y+1]=='x') score++;
					if (screen[x][y-1]=='x') score++; }
				if (screen[x][y]==' ' && screen[x][y+1]=='x') score++;
				if (screen[x][y]==' ' && screen[x][y-1]=='x') score++;
			}
			if (x==0){ hb=1; x+=2; }
		}
    }
}	
/***************--FONCTION ESSENTIELLE--*********************/
void play(){ 
	int key;  int vitesse;
	switch(level){ //~ Pour chaque niveau, on a une vitesse du jeu correspondante ( et croissante respectivement )
		case 1: { vitesse = 120; break; }
		case 2: { vitesse = 100; break; }
		case 3: { vitesse = 80; break; }
	}
	do{
		if (start==false) brique_display();
		if (level==4) finish_win(); //~ On a seulement 3 niveaux du jeu.
      
		int cpt=0; 
		for (int i=0; i<H-2; i++)
			for (int j=0; j<L; j++){ 
				if (screen[i][j]=='o'){	screen[i][j]=' '; screen[i][j-1]=' '; screen[i][j+1]=' '; }
				//~ L'influence de la balle
				if (screen[i][j]=='x') cpt++;
		}
		
		//~ Si les briques sont tout detruites, on va passer le niveau
		if (cpt==0){ 
			level+=1;
			start=false;
			x=H-3; y=L/2;
			for (int j=0;j<L;j++)
				screen[H-2][j]=' ';
		}
 
		key=read_keyboard();
		if (key=='p') introduction(); //~ une methode pour pauser le jeu et regarder la tutoriel si vous voulez
		if (key=='s') start=true;	//~ prenez [s] pour commencer le jeu!			
		clear_screen();
		frontier();
		barreB(key);
		update_game(key);
		screen_display();     
		usleep(vitesse*1000);
	}
	while (key!='q');
	exit(0);
}

//~ Rejouer... pourquoi pas avec tel jeu comme-ci?!
void replay(){
	clear_screen();
	level = 1;
	score = 0;
	start=false; 
	x=H-3; y=L/2; hb=0;
	for (int j=0;j<L;j++)
		screen[H-2][j]=' ';
	play();
}

//~ Tutoriel, si vous voulez
void tutorial(){
	system("clear");
	cout << "Utilisez" << RED << " [a] " RESET << "ou" << RED << " [d] " << RESET << "pour deplacer la raquette." << endl;
	cout << "Prenez" << GREEN << " [q] " << RESET << "si vous voulez quitter le jeu." << endl;
	cout << "Prenez" << MAGENTA << " [s] " << RESET << "pour commencer le jeu" << endl;
	cout << "Prenez" << YELLOW << " [p] " << RESET << "si vous voulez pauser/resumer le jeu." << endl << endl;
	cout << "Prenez [ENTER] quelconque pour" << BLUE << " JOUERRRRRR/CONTINUERRRRR" << RESET;
	cin.get();
	play();
}

//~ L'introduction du jeu
void introduction(){
	system("clear");
	cout << YELLOW << "\t\t<--BIENVENUE AUX CASSE-BRIQUES!!!-->" << RESET << endl << endl;
	int key;
	cout << "\tCASSE-BRIQUES est un jeu de vidéo du type d'arcarde. Le principe général est de détruire, ";
	cout << "au moyen d'une ou plusieurs balles, un ensemble de briques se trouvant dans un niveau pour ";
	cout << "accéder au niveau suivant. le joueur contrôle une sorte de raquette qu'il peut seulement ";
	cout << "déplacer sur un axe horizontal au bas de l'écran, et le but est d'empêcher la balle de ";
	cout << "franchir cette ligne en la frappant avec la raquette." << endl << endl << endl;
	cout << "On a 3 niveaux du jeu avec les difficultés croissantes respectivement!"<< endl << endl;
	if(start==false)	cout << "\t\t[1] Jouer" << endl;
	else cout << "\t\t[1] Rejouer" << endl;
	cout << "\t\t[2] Tutoriel" << endl;
	cout << "\t\t[3] Quitter" << endl << endl;
	cout << "Prenez un des boutons ci-dessus pour continuer" << endl << endl;
	if(start==true)	cout << "Prenez [p] pour continuer / [1] pour rejouer";
	do{
		key = read_keyboard();
		if(start == false && key == '1') play();
		if(start == true && key == '1') replay();
		if(key == '2') tutorial();
		if(key == '3') system("clear");
		if(start == true && key == 'p') play();
	}while (key != '3');
}

int main(){
	introduction();
	return EXIT_SUCCESS;
}
/*****************************************************************************/
//~ Fonctions pourque l'on puisse prendre des boutons
int kbhit(void){
	struct termios oldt, newt;
    int ch, oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF){
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

int read_keyboard(){
	int key;
	if (kbhit())
		key = getc(stdin);
	else
		key = 0;
	return key;
}

/******************************************************************************/


