#include <queue>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

bool win, lose;
sf::Vector2i size[] = { {11,11}, {18,18}, {32,18} };
int bombSize[] = { 10, 40, 99 };

char difficulty;
int sizeX, sizeY;

struct Cell 
{ 
    int type; 
    bool isOpened;
    bool isPawned;
};
std::vector<std::vector<Cell>> map;

sf::Vector2i Dir[] = {
    {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}
};

sf::Texture textures[12];
std::vector<sf::Sprite> sprites;

//Data Process...
void bombGen()
{
    int bombQuantity = 0, x, y, _x, _y;
    bool bombaround;
    while (bombQuantity < bombSize[difficulty])
    {
        x = rand() % sizeX + 1;
        y = rand() % sizeY + 1;
        bombaround = true;

        if (map[y][x].type != 9)
        { 
            for (int d = 0; d < 8; d++)
            {
                _x = x + Dir[d].x;
                _y = y + Dir[d].y;
                if (map[_y][_x].type != 9 and map[_y][_x].type != -1)
                {
                    map[_y][_x].type++;
                    bombaround = false;
                }
            }                
        }
        if (!bombaround)
        {
            map[y][x].type = 9;
            bombQuantity++;
        }
    }
}

void mapSet()
{
    for (int i = 0; i < sizeY + 2; i++)
    {
        for (int j = 0; j < sizeX + 2; j++)
        {
            if ((i == 0 or i == sizeY + 1) or (j == 0 or j == sizeX + 1))
                map[i][j].type = -1;
            else
                map[i][j].type = 0;
            map[i][j].isOpened = false;
            map[i][j].isPawned = false;
        }
    }
}

void MultiOpen(int x, int y)
{
    int _x, _y;

    std::queue<sf::Vector2i> nowPos;
    nowPos.push({ x, y });
    map[y][x].isOpened = true;
    while (!nowPos.empty())
    {
        sf::Vector2i pos = nowPos.front();
        nowPos.pop();
        for (int d = 0; d < 8; d++)
        {
            _x = pos.x + Dir[d].x;
            _y = pos.y + Dir[d].y;
            if (!map[_y][_x].isPawned and !map[_y][_x].isOpened)
            {
                if (map[_y][_x].type == 0)
                    nowPos.push({ _x, _y });
                if (map[_y][_x].type != 9 and map[_y][_x].type != -1)
                    map[_y][_x].isOpened = true;
            }    
        }
    }
}

bool winDetect()
{
    int num = 0;
    for (int i = 1; i <= sizeY; i++)
    {
        for (int j = 1; j <= sizeX; j++)
        {
            if (map[i][j].isOpened)
                num++;
            if (num >= sizeY * sizeX - bombSize[difficulty])
                return true;
        }   
    }

    return false;
}

//Render Process...
sf::Sprite setSprite(sf::Texture &tex, int x, int y)
{
    sf::Sprite spr;
    spr.setTexture(tex);
    spr.setScale({ 32.f / 200, 32.f / 200 });
    spr.setPosition({ x * 32.f, y * 32.f});
    return spr;
}

void spriteUpdate() 
{
    for (int i = 0; i < sizeY; i++)
    {
        for (int j = 0; j < sizeX; j++)
        {
            if (!map[i + 1][j + 1].isOpened)
            {
                if (map[i + 1][j + 1].isPawned)
                    sprites[sizeX * i + j] = setSprite(textures[11], j, i);
                else
                    sprites[sizeX * i + j] = setSprite(textures[10], j, i);
            }  
            else
                sprites[sizeX * i + j] = setSprite(textures[map[i + 1][j + 1].type], j, i);
        }
    }
}

bool initialize()
{
    std::string images[] = {
        "0.png",
        "1.png", "2.png", "3.png", "4.png", "5.png", "6.png", "7.png", "8.png",
        "bomb.png",
        "facingDown.png",
        "flagged.png"
    };

    for (int i = 0; i < 12; i++)
        if (!textures[i].loadFromFile("Images/" + images[i]))
            return false;

    return true;
}

//start or restart...
void start(sf::RenderWindow &window, sf::Clock &clock)
{
    std::cout << "difficulty: ";
    std::cin >> difficulty;
    if (!(difficulty == '0' or difficulty == '1' or difficulty == '2'))
    {
        std::cout << "Please enter the correct difficulty!\n";
        return start(window, clock);
    }
        
    difficulty -= '0';

    win = false; lose = false;
    sizeX = size[difficulty].x - 2;
    sizeY = size[difficulty].y - 2;

    sprites.resize((sizeY) * (sizeX));
    map.resize(sizeY + 2);
    for (std::vector<Cell> &i : map)
        i.resize(sizeX + 2);

    mapSet();
    bombGen();
    spriteUpdate();
    window.create(sf::VideoMode(32 * sizeX, 32 * sizeY), "SFML");
    window.setFramerateLimit(60);
    clock.restart();
}

//main function...
int main()
{
    srand(time(NULL));
    if (!initialize())
        return -1;

    sf::RenderWindow window;
    sf::Clock clock;
    sf::Time time;

    std::cout << "Please enter the difficulty below.\n0 for easy\n1 for normal\n2 for hard\n\n";
    start(window, clock);
    
    while (window.isOpen())
    {
        sf::Event event;
        
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i pos = sf::Mouse::getPosition();
                sf::Vector2i corr = window.getPosition();
                int x = (pos.x - corr.x - 8) / 32 + 1;
                int y = (pos.y - corr.y) / 32;

                if ((x >= 1 and x <= sizeX) and (y >= 1 and y <= sizeY))
                {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) and !map[y][x].isPawned)
                    {
                        if (map[y][x].type != 0 and map[y][x].type != 9)
                            map[y][x].isOpened = true;
                        else if (map[y][x].type == 0)
                            MultiOpen(x, y);
                        else if (map[y][x].type == 9)
                        {
                            map[y][x].isOpened = true;
                            lose = true;
                        }
                    }
                        
                    else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                        map[y][x].isPawned = !map[y][x].isPawned;

                    win = winDetect();
                    spriteUpdate();
                }
            }
        }

        //render
        window.clear(sf::Color(128,128,128,0));
        for (sf::Sprite i : sprites)
            window.draw(i);
        window.display();

        //timer
        time = clock.getElapsedTime();
        printf("%d:%2d\r", (int)time.asSeconds() / 60, (int)time.asSeconds() % 60);

        //Game state detect
        if (win)
        {
            std::cout << "You win!\n";
            printf("Time you cost: %d:%2d\n", (int)time.asSeconds() / 60, (int)time.asSeconds() % 60);
            sf::sleep(sf::seconds(2));
            window.close();
            start(window, clock);
        }
        else if (lose)
        {
            std::cout << "You lose!\n";
            sf::sleep(sf::seconds(2));
            window.close();
            start(window, clock);
        }  
    }

    return 0;
}