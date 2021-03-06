
#pragma once

#include "Game.h"

#include <iostream>
#include <SFML\Graphics.hpp>
#define MAX_NUMBER_OF_ITEMS 3


//Contains an option from the main menu
enum MainMenuOption
{
	INSTRUCTIONS,
	PLAY_GAME,
	NOTHING,
	EXIT
};

struct button {
    sf::RectangleShape Shape;
    int buttonW;
    int buttonH;
    bool is_over(sf::RenderWindow& window);
};

class Menu
{
private:
    MainMenuOption selectedItemIndex;
	sf::Font font;
	sf::Text text[MAX_NUMBER_OF_ITEMS];
	button menu_buttons[3];

public:
	Menu(float width, float height);
	void display_main_menu(sf::RenderWindow& window);
	void mousePlace(sf::RenderWindow& window);
	void get_main_menu_option(sf::RenderWindow& window);
	MainMenuOption get_Selected_Index();
	void set_Selected_Index(MainMenuOption option);

    void display_instructions(sf::RenderWindow& window);
    void play_game(sf::RenderWindow& window);
};



