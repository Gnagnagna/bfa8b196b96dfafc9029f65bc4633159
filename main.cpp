//Les fichiers d'entête
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <SDL/SDL.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SDL/SDL_ttf.h"
//Les attributs de l'écran (640 * 480)
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//Le nombre de frame par seconde
const int FRAMES_PER_SECOND = 30;

//Les dimensions du point (l'image qu'on va deplacer)
const int POINT_WIDTH = 137;
const int POINT_HEIGHT = 90;
int POINT_ETAT=1;
const int Largeur_Otage = 15;
const int Hauteur_Otage = 30;


//Vitesse de deplacement du point (en pixel par seconde)
const int POINT_VITESSE = 600;

//Les dimensions du niveau
const int LEVEL_WIDTH = 5000;
const int LEVEL_HEIGHT = 600;

//Les surfaces
SDL_Surface *point = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *otagesurface =NULL;
SDL_Surface *message = NULL;
SDL_Surface *missile=NULL;


//La structure d'événements que nous allons utiliser
SDL_Event event;
SDL_Rect clip[ 4 ];
SDL_Rect otage[3];
//Le Font qu'on va utiliser
TTF_Font *font;

//La couleur du Font
SDL_Color textColor = { 125, 231, 92 };

//La camera
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
int rand_a_b(int a, int b){
    return rand()%(b-a) +a;
}
///////////////////Classe pour le missile//////////////////////

///////////////////////////////////////////////////////////////
class Point
{
    private:
    //Les coordonnées x et y du point
    int x, y;

    //La vitesse du point
    int xVel, yVel;

    public:
    //Initialisation des variables
    Point();

    //Recupere la touche pressee et ajuste la vitesse du point
    void handle_input();

    //Bouge le point
    void move();

    //Montre le point sur l'ecran
    void show();

    //Met la camera sur le point
    void set_camera();
};
class Otage
{
    private:
    //Les coordonnées x et y du point
    int x_Otage, y_Otage,x_Dep_Otage;


    //La vitesse du point
    int xVel_Otage, yVel_Otage;
    int OTAGE_ETAT;

    public:
    //Initialisation des variables
    Otage(int x_otage);

    //Recupere la touche pressee et ajuste la vitesse du point
    void handle_input();

    //Bouge le point
    void move();

    //Montre le point sur l'ecran
    void show();

    //Met la camera sur le point
    void set_camera();
};

//Le timer
class Timer
{
    private:
    //Le temps quand le timer est lancé
    int startTicks;

    //Les "ticks" enregistré quand le Timer a été mit en pause
    int pausedTicks;

    //Le status du Timer
    bool paused;
    bool started;

    public:
    //Initialise les variables (le constructeur)
    Timer();

    //Les différentes actions du timer
    void start();
    void stop();
    void pause();
    void unpause();

    //recupére le nombre de ticks depuis que le timer a été lancé
    //ou récupére le nombre de ticks depuis que le timer a été mis en pause
    int get_ticks();

    //Fonctions de vérification du status du timer
    bool is_started();
    bool is_paused();
};

SDL_Surface *load_image( std::string filename )
{
    //L'image qui est chargée
    SDL_Surface* loadedImage = NULL;

    //L'image optimisée qu'on va utiliser
    SDL_Surface* optimizedImage = NULL;

    //Chargement de l'image
    loadedImage = IMG_Load( filename.c_str() );

    //Si l'image a bien chargée
    if( loadedImage != NULL )
    {
        //Création de l'image optimisée
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Libération de l'ancienne image
        SDL_FreeSurface( loadedImage );

        //Si la surface a bien été optimisée
        if( optimizedImage != NULL )
        {
            SDL_SetColorKey( optimizedImage, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 153,217,234 ) );
        }
    }

    //On retourne l'image optimisée
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    SDL_Rect offset;

    //Recuperation des coordonnées
    offset.x = x;
    offset.y = y;

    //On blitte la surface
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialisation de tous les sous-système de SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Mise en place de l'écran
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //S'il y a eu une erreur lors de la mise en place de l'écran
    if( screen == NULL )
    {
        return false;
    }
//Initialisation de SDL_TTF
	if( TTF_Init() == -1 ) {
		return false;
	}
    //Mise en place de la barre caption
    SDL_WM_SetCaption( "Choplifter", NULL );

    //Si tout s'est bien passé
    return true;
}

bool load_files()
{
    //Chargement du point
    point = load_image( "spritehelico.png" );

    //Chargement du fond
    background = load_image( "carte2.bmp" );
    otagesurface=load_image("otage.png");
    missile=load_image("missile.png");
font = TTF_OpenFont( "CaslonBold.ttf", 28 );

	//S'il y a un problème au chargement du fond
	if( background == NULL ) {
		return false;
	}

	//S'il y a une erreur dans le chargement du Font
	if( font == NULL ) {
		return false;}
    //S'il y a eu un problème au chargement du point
    if( point == NULL )
    {
        return false;
    }

    //S'il y a eu un problème au chargement du fond
    if( background == NULL )
    {
        return false;
    }

    //Si tout s'est bien passé
    return true;
}

void clean_up()
{
    //Libèration des surfaces
    SDL_FreeSurface( point );
    SDL_FreeSurface( background );
    SDL_FreeSurface(otagesurface);
    SDL_FreeSurface(missile);
TTF_CloseFont( font );

	//On quitte SDL_ttf
	TTF_Quit();
    //On quitte SDL
    SDL_Quit();
}


Otage::Otage(int x_otage)
{
    //Initialisation des coordonnees
    x_Otage = x_otage;
    x_Dep_Otage=x_otage;
    y_Otage = 567;
    OTAGE_ETAT=1;

    //Initialisation de la vitesse
    xVel_Otage = rand_a_b(3,4);
    yVel_Otage = 0;
}


Point::Point()
{
    //Initialisation des coordonnees
    x = 0;
    y = 0;

    //Initialisation de la vitesse
    xVel = 0;
    yVel = 0;
}
void Otage::move()
{   if(x_Otage>=(x_Dep_Otage+500))
        OTAGE_ETAT=2;
    if(x_Otage<=(x_Dep_Otage-500))
        OTAGE_ETAT=1;
    if (OTAGE_ETAT==1)
    {   //Bouge le point à gauche ou à droite
    x_Otage += xVel_Otage;

    //Si le point se rapproche trop des limites(gauche ou droite) de l'ecran
    if( ( x_Otage < 0 ) || ( x_Otage + Largeur_Otage > LEVEL_WIDTH ) )
    {
        //On revient
        x_Otage -= xVel_Otage;
        OTAGE_ETAT=2;
    }
    }
    if (OTAGE_ETAT==2)
    {   //Bouge le point à gauche ou à droite
    x_Otage -= xVel_Otage;

    //Si le point se rapproche trop des limites(gauche ou droite) de l'ecran
    if( ( x_Otage < 0 ) || ( x_Otage + Largeur_Otage > LEVEL_WIDTH ) )
    {
        //On revient
        x_Otage += xVel_Otage;
        OTAGE_ETAT=1;
    }
    }



}

void Point::handle_input()
{
    //Si une touche a ete pressee
    if( event.type == SDL_KEYDOWN )
    {
        //ajustement de la vitesse
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel -= (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=POINT_ETAT%2; break;
            case SDLK_DOWN: yVel += (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=POINT_ETAT%2; break;
            case SDLK_LEFT: xVel -= (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=2; break;
            case SDLK_RIGHT: xVel += (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=3;  break;
            default: break;
        }
    }
    //Si une touche a ete relachee
    else if( event.type == SDL_KEYUP )
    {
        //ajustement de la vitesse
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel += (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=POINT_ETAT%2; break;
            case SDLK_DOWN: yVel -= (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=POINT_ETAT%2; break;
            case SDLK_LEFT: xVel += (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=POINT_ETAT%2; break;
            case SDLK_RIGHT: xVel -= (POINT_VITESSE / FRAMES_PER_SECOND);POINT_ETAT=POINT_ETAT%2; break;
            default: break;
        }
    }
}

void Point::move()
{
    //Bouge le point à gauche ou à droite
    x += xVel;

    //Si le point se rapproche trop des limites(gauche ou droite) de l'ecran
    if( ( x < 0 ) || ( x + POINT_WIDTH > LEVEL_WIDTH ) )
    {
        //On revient
        x -= xVel;
    }

    //Bouge le point en haut ou en bas
    y += yVel;

    //Si le point se rapproche trop des limites(haute ou basse) de l'ecran
    if( ( y < 0 ) || ( y + POINT_HEIGHT > LEVEL_HEIGHT ) )
    {
        //On revient
        y -= yVel;
    }
}
void Otage::show()
{
    //Affiche le point
    apply_surface( x_Otage- camera.x, y_Otage - camera.y, otagesurface, screen,&otage[OTAGE_ETAT] );
}

void Point::show()
{
    //Affiche le point
    apply_surface( x - camera.x, y - camera.y, point, screen,&clip[POINT_ETAT] );
}

void Point::set_camera()
{
    //Centre la camera sur le point
    camera.x = ( x + POINT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
    camera.y = ( y + POINT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

    //Garde la camera sur les bords
    if( camera.x < 0 )
    {
        camera.x = 0;
    }
    if( camera.y < 0 )
    {
        camera.y = 0;
    }
    if( camera.x > LEVEL_WIDTH - camera.w )
    {
        camera.x = LEVEL_WIDTH - camera.w;
    }
    if( camera.y > LEVEL_HEIGHT - camera.h )
    {
        camera.y = LEVEL_HEIGHT - camera.h;
    }
}

Timer::Timer()
{
    //Initialisation des variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //On demarre le timer
    started = true;

    //On enlève la pause du timer
    paused = false;

    //On récupére le temps courant
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    //On stoppe le timer
    started = false;

    //On enlève la pause
    paused = false;
}

void Timer::pause()
{
    //Si le timer est en marche et pas encore en pause
    if( ( started == true ) && ( paused == false ) )
    {
        //On met en pause le timer
        paused = true;

        //On calcul le pausedTicks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //Si le timer est en pause
    if( paused == true )
    {
        //on enlève la pause du timer
        paused = false;

        //On remet à zero le startTicks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Remise à zero du pausedTicks
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    //Si le timer est en marche
    if( started == true )
    {
        //Si le timer est en pause
        if( paused == true )
        {
            //On retourne le nombre de ticks quand le timer a été mis en pause
            return pausedTicks;
        }
        else
        {
            //On retourne le temps courant moins le temps quand il a démarré
            return SDL_GetTicks() - startTicks;
        }
    }

    //Si le timer n'est pas en marche
    return 0;
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}



int main(int argc, char **argv)
{int i;
srand(time(NULL)); // initialisation de rand
message = TTF_RenderText_Solid( font, "Test pour sdl_ttf", textColor );
srand(time(NULL));
int tableauOtage[20];


    //Ce qui va nous permettre de quitter
    bool quit = false;

    //Le point qu'on va utilise
    Point monPoint;
    Otage otage_1(100);
    Otage otage_2(200);




    //Le regulateur
    Timer fps;

    //Initialisation

    if( init() == false )
    {printf("int");
        return 1;}

    //Chargement des fichiers
    if( load_files() == false )
    {printf("load");
        return 1;}
    //Sprite de l'hélico
    clip[ 0 ].x = 0;
    clip[ 0 ].y = 0;
    clip[ 0 ].w = POINT_WIDTH;
    clip[ 0 ].h = POINT_HEIGHT;

    clip[ 1 ].x = POINT_WIDTH;
    clip[ 1 ].y = 0;
    clip[ 1 ].w = POINT_WIDTH;
    clip[ 1 ].h = POINT_HEIGHT;

    //On coupe la partie en bas à gauche (troisième sprite)
    clip[ 2 ].x = 0;
    clip[ 2 ].y = POINT_HEIGHT;
    clip[ 2 ].w = POINT_WIDTH;
    clip[ 2 ].h = POINT_HEIGHT;

    otage[0].x=0;
    otage[0].y=0;
    otage[0].w=15;
    otage[0].h=30;
    otage[1].x=15;
    otage[1].y=0;
    otage[1].w=15;
    otage[1].h=30;
    otage[2].x=30;
    otage[2].y=0;
    otage[2].w=15;
    otage[2].h=30;


    //On coupe la partie en bas à droite (quatrième sprite)
    clip[ 3 ].x = POINT_WIDTH;
    clip[ 3 ].y = POINT_HEIGHT;
    clip[ 3 ].w = POINT_WIDTH;
    clip[ 3 ].h = POINT_HEIGHT;

    //Tant que l'utilisateur n'a pas quitter
    while( quit == false )
    {
        //On demarre le timer fps
        fps.start();

        //Tant qu'il y a un événement
        while( SDL_PollEvent( &event ) )
        {
            //On recupere l'evenement pour le point
            monPoint.handle_input();

            //Si l'utilisateur a cliqué sur le X de la fenêtre
            if( event.type == SDL_QUIT )
            {
                //On quitte the programme
                quit = true;
            }
        }

        //Bouge le point
        monPoint.move();

        otage_1.move();
        otage_2.move();




        //Met la camera
        monPoint.set_camera();

        //Affiche le fond
        apply_surface( 0, 0, background, screen, &camera );


        //Affiche le point sur l'écran
        monPoint.show();
        otage_1.show();
        otage_2.show();



        //Mise à jour de l'écran
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        //Tant que le timer fps n'est pas assez haut
        while( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            //On attend...
        }
    }

    //Nettoyage
    clean_up();

    return 0;
}
