
#include "Board.h"

sf::RectangleShape* createGrid(double width, double height, double cellSize) {

    //Number of squares across and down
    int x = width / cellSize, y = height / cellSize, total = x * y;

    double xCoord = 0, yCoord = 0;

    sf::RectangleShape* squareGrid = new sf::RectangleShape[total];

    for (int m = 0; m < total; m++, xCoord += cellSize) {
        squareGrid[m].setSize(sf::Vector2f(cellSize, cellSize));
        squareGrid[m].setFillColor(sf::Color::Black);
        squareGrid[m].setOutlineThickness(2);
        squareGrid[m].setOutlineColor(sf::Color::White);
        if (m % x == 0 && m != 0) {
            xCoord = 0;
            yCoord += cellSize;
        }
        squareGrid[m].setPosition(sf::Vector2f(xCoord, yCoord));
    }

    return squareGrid;
}
void Board::createSpriteGrid(double width, double height, double cellSize)
{
	//Number of squares across and down
	int x = width / cellSize, y = height / cellSize, total = x * y;

	double xCoord = 0, yCoord = 0;

	spriteGrid = new sf::Sprite[total];

	for (int m = 0; m < total; m++, xCoord += cellSize) {
		if (m % x == 0 && m != 0) {
			xCoord = 0;
			yCoord += cellSize;
		}
		spriteGrid[m].setPosition(sf::Vector2f(xCoord, yCoord));
	}
}
Board::Board()
{
	gridWidth = GRID_WIDTH;
	gridHeight = GRID_HEIGHT;
	cellSize = CELL_SIZE;
	//grid = createGrid(gridWidth, gridHeight, cellSize);
	columns = (int)(gridWidth / cellSize);
	rows = (int)(gridHeight / cellSize);
	pathLength = 0;
	squareCount = columns * rows; //(width / cellsize) * (width / cellsize)
	readPath("Resources/path.csv");

	//Load textures
	if (!tower.loadFromFile("Resources/towerResized.png"))
		cout << "Error: Tower texture file not loaded.\n";
	if (!dirt.loadFromFile("Resources/dirtSquareResized.png"))
		cout << "Error: Dirt texture file not loaded.\n";
	if (!grass.loadFromFile("Resources/grassSquareResized.png"))
		cout << "Error: Grass texture file not loaded.\n";
	if (!castle.loadFromFile("Resources/castleResized.png"))
		cout << "Error: Castle texture file not loaded.\n";
	createSpriteGrid(gridWidth, gridHeight, cellSize);
	drawTextures();
}
void Board::draw(sf::RenderWindow& window)
{
	//Draws grid on board. Must be used BEFORE any calls of window.draw()
	//or it will hide other objects
	for (int i = 0; i < squareCount; ++i) {
		window.draw(spriteGrid[i]);
	}
}
void Board::markPath()
{
	//Draws path on board in red. Marks player castle yellow 
	//Called by drawTextures() 
	for (int i = 0; i < pathLength; ++i) {
		//grid[path[i]].setFillColor(sf::Color::Red);
		spriteGrid[path[i]].setTexture(dirt);
		//if(i == pathLength - 1)
		//	grid[path[i]].setFillColor(sf::Color::Yellow);
		if (i == pathLength - 1)
			spriteGrid[path[i]].setTexture(castle);
	}
}
void Board::readPath(string fileName)
{
	//Note that grid is a 1D array, not 2D which would be preferrable 
	//Therefore path values are just big numbers
	//Path.csv is dependent on current grid size
	//Can be used with other files for different paths
	//Any csv file used must have all values on a single line and end with a comma
	//See path.csv for reference
	
	fstream inFile;
	inFile.open(fileName, fstream::in);
	if (!inFile) {
		cout << fileName << " failed to open.\n";
		path = 0; //Ensures path is initialized
	}
	else {
		string buff;
		getline(inFile, buff);
		int commaCount = 0, i = 0;
		for (i = 0; i < buff.length(); i++) {
			if (buff[i] == ',')
				++commaCount;
		}
		path = new int[commaCount];
		stringstream s_stream(buff);
		string substr;
		i = 0;
		while (s_stream.good()) {
			getline(s_stream, substr, ',');
			if (substr == "")
				break;
			*(path + i) = stoi(substr);
			++i;
		}
		pathLength = commaCount;
	}
	inFile.close();

}
bool Board::inPath(int square) 
{
	//Determines if a square is in the enemy marching path
	//Can be used for checking if a square is a valid location for a tower
	for (int i = 0; i < pathLength; ++i) {
		if (square == path[i])
			return true;
	}
	return false;
}
Board::~Board()
{
	delete[] path;
	delete[] grid;
}
int Board::getSquareCoord(double x, double y)
{
	int column = 0, row = 0;
	int cellNumber = 0;

	if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
		row = y / cellSize;
		column = x / cellSize;
		cellNumber = (row * columns) + column;
		return cellNumber;
	}
	else
		return -1;
}
void Board::colorCell(int cellNum)
{
	grid[cellNum].setFillColor(sf::Color::Blue); 
}
int Board::addTower(sf::Vector2f position)
{
	int cell = getSquareCoord(position.x, position.y);
	if (cell == -1)
		return -1;
	position = spriteGrid[cell].getPosition();
	int result = 1;
	//Check that not in path
	if (inPath(cell)) {
		cout << "Cell is in enemy path.\n";
		result = 0;
	}
	else {
		//Check that no tower there
		if (!isOpen(position)) {
			cout << "Cell is occupied.\n";
			result = -1;
		}
		else {
			//Add tower 
			Tower newTower(position);
			towers[towerCount] = newTower;
			spriteGrid[cell].setTexture(tower);
			++towerCount;
		}
	}
	return result;
}
bool Board::isOpen(sf::Vector2f position)
{
	if (towerCount == 0)
		return true;
	for (int i = 0; i < towerCount; ++i) { 
		if ((towers + i)->getPosition() == position)
			return false;
	}
	return true;
}
sf::Vector2f Board::getStartingPosition(void)
{
	//Lazy way: just report default starting point
	return sf::Vector2f(387, 0);
	//Better way: report first square on path
	//Won't work as a static function
}
Direction Board::getDirection(sf::Vector2f position)
{
	int cell = getSquareCoord(position.x, position.y);
	int index = -1;
	for (int i = 0; i < pathLength; ++i) {
		if (path[i] == cell)
			index = i;
	}
	if (index == -1) {
		cout << "Coordinates not in path.\n";
		return DOWN;
	}
	//Down
	if (path[index + 1] - 20 == path[index])
		return DOWN;
	//Up
	if (path[index + 1] + 20 == path[index])
		return UP;
	//Right
	if (path[index + 1] - 1 == path[index])
		return RIGHT;
	//Left
	if (path[index + 1] + 1 == path[index])
		return LEFT;
	cout << "Path could not be determined.\n";
	return DOWN;
}
void Board::drawTextures()
{
	for (int i = 0; i < squareCount; ++i) {
		spriteGrid[i].setTexture(grass);
	}
	markPath();
}

bool Board::is_at_castle(sf::Vector2f position)
{
    return false; //temp
}

