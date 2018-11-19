#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>

//toggle a shapes color for a flashing effect
void flashShape(sf::RectangleShape&,sf::RenderWindow&,unsigned,unsigned);
//add an "enemy" to the board
void addEnemy(std::vector<sf::RectangleShape*>&,unsigned,unsigned,unsigned,unsigned);
//move the player character
void movePlayer(sf::RectangleShape&,unsigned&,unsigned&,unsigned,unsigned,unsigned,unsigned);
//draw the game board
void drawGame(sf::RenderWindow&,sf::RectangleShape&,std::vector<sf::RectangleShape*>&,unsigned);
//detect a collision and set the lose state if necessary
void collisionDetect(sf::RenderWindow&,sf::RectangleShape&,std::vector<sf::RectangleShape*>&,bool&);
//displpay a background card
void backgroundCardDisplay(sf::RenderWindow&,sf::Texture&);

int main()
{
    /*---some game globals---*/
    const unsigned X_BOUND = 800; //the window bounds
    const unsigned Y_BOUND = 800; //the window bounds
    const unsigned GAME_LENGTH = 15; //the length of time for the game in seconds
    const float MOVE_SPEED = 325; //the movement speed of the enemy in milliseconds
    const unsigned SLEEP_MS = 1000; //the time to sleep each iteration in milliseconds
    bool tagged=false; //has the player been tagged/lost yet?

    /*---set up timers sprites and windows---*/
    sf::RenderWindow window(sf::VideoMode(X_BOUND, Y_BOUND), "Box Game! :D");
    sf::Texture howtoplay; //different textures for the sprite
    sf::Texture win;
    sf::Texture lose;
    win.loadFromFile("WinScreen.png");
    lose.loadFromFile("LoseScreen.png");
    howtoplay.loadFromFile("IntroScreen.png");
    backgroundCardDisplay(window,howtoplay);

    srand (time(NULL));//seed the rng
    sf::Clock clock; //the main clock
    sf::Clock move; //clock for enemy moves
    sf::Time elapsed = clock.getElapsedTime(); //main elapsed time game ends based on this
    sf::Time move_time = move.getElapsedTime(); //enemy move elapsed time

    /*---player char setup---*/
    const unsigned PLAYER_HT = 20; //the height of the player
    const unsigned PLAYER_WT= 20; //width of the player
    unsigned x = 0; //tracks players x coord
    unsigned y = 0; //tracks players y coord
    sf::RectangleShape shape(sf::Vector2f(PLAYER_WT,PLAYER_HT)); //build the player
    shape.setFillColor(sf::Color::Green); //set the player color

    /*---enemy setup---*/
    const unsigned BOX_HT = 200; //used to calc the height of the enemy
    const unsigned BOX_WT= 200; //used to calc the width of the enemy
    std::vector<sf::RectangleShape*> enemyvector; //a vector of shapes for the enemies
    addEnemy(enemyvector,rand()%X_BOUND,rand()%Y_BOUND,BOX_HT,BOX_WT); //add teh first enemy

    shape.setPosition(x,y); //set player pos
    drawGame(window,shape,enemyvector,SLEEP_MS); //draw first screen
    while (elapsed<=sf::seconds(GAME_LENGTH))
    {
        //move the player
        movePlayer(shape,x,y,X_BOUND,Y_BOUND,PLAYER_HT,PLAYER_WT);

        //move enemy
        if(int(move_time.asMilliseconds())>=MOVE_SPEED)
        {
            addEnemy(enemyvector,rand()%X_BOUND,rand()%Y_BOUND,BOX_HT,BOX_WT);
            move.restart();
        }

        //redraw window
        drawGame(window,shape,enemyvector,SLEEP_MS);

        //check if enemy and player touch
        collisionDetect(window,shape,enemyvector,tagged);
        if (tagged) break;

        //refresh the timers
        elapsed = clock.getElapsedTime();
        move_time = move.getElapsedTime();
    }

    //draw final game state
    drawGame(window,shape,enemyvector,SLEEP_MS);
    ::usleep(1000000);

    //print win/lose info
    if (!tagged) backgroundCardDisplay(window,win);
    else backgroundCardDisplay(window,lose);

    //close the window
    window.close();

    //clean up
    for (auto it: enemyvector) delete it;

    //program ends
    return 0;
}
void flashShape(sf::RectangleShape& sp, sf::RenderWindow& window,unsigned speed,unsigned length)
{
    for (unsigned i =0; i<length; i++)
    {
        sp.setFillColor(sf::Color::Black);
        window.draw(sp);
        window.display();
        usleep(speed);
        sp.setFillColor(sf::Color::Red);
        window.draw(sp);
        window.display();
        usleep(speed);
    }
}
void addEnemy(std::vector<sf::RectangleShape*>& vec,unsigned x,unsigned y,unsigned ht,unsigned wt)
{
    vec.push_back(new sf::RectangleShape(sf::Vector2f(rand()%ht,rand()%wt))); //build the first one
    vec[vec.size()-1]->setFillColor(sf::Color(rand()%255,rand()%255,rand()%255)); //set its color
    vec[vec.size()-1]->setPosition(x,y);//set its position
}
void movePlayer(sf::RectangleShape& shape,unsigned& x,unsigned& y,unsigned xbound,unsigned ybound,unsigned pht,unsigned pwt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)&&y>0) y=(y-1);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)&&x>0) x=(x-1);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)&&y<ybound-pht) y=(y+1);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)&&x<xbound-pwt) x=(x+1);
    shape.setPosition(x,y);
}
void drawGame(sf::RenderWindow& win,sf::RectangleShape& shp,std::vector<sf::RectangleShape*>& vec,unsigned stime)
{
    win.draw(shp);
    for (auto it: vec) win.draw(*it);
    win.display();
    ::usleep(stime);
    win.clear();
}
void collisionDetect(sf::RenderWindow& win,sf::RectangleShape& shp,std::vector<sf::RectangleShape*>& vec,bool& hit)
{
    for (auto it: vec)
    {
        if (shp.getGlobalBounds().intersects(it->getGlobalBounds()))
        {
            hit=true;
            std::cout << "KILLING ENEMY POSITION: (x,y): (" << it->getPosition().x<<","<<it->getPosition().y<<")"<<std::endl;
            std::cout << "YOUR POSITION: (x,y): (" << shp.getPosition().x<<","<<shp.getPosition().y<<")"<<std::endl;
            flashShape(*it,win,100000,12);
        }
    }
}
void backgroundCardDisplay(sf::RenderWindow& win,sf::Texture& card)
{
    sf::Sprite background;
    background.setTexture(card);
    win.draw(background);
    win.display();
    ::sleep(4);
    background.setColor(sf::Color(255, 255, 255, 200));

}
