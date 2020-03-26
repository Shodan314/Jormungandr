#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#define msize 30
#define csize 30
#define maxtime 100
#define win 100

using namespace std;
using namespace sf;

random_device rng;
uniform_int_distribution < int > RNGrange(0, msize - 1);

class Cell {
    public:
    pair < int, int > coordinates;
    Cell() {}
    Cell(int x, int y) {
        coordinates.first = x;
        coordinates.second = y;
    }~Cell() {}
};

class Matrix {
    public:
    vector < Cell > cells;
    Matrix() {
        for (int i = 0; i < msize; i++)
            for (int j = 0; j < msize; j++)
                cells.push_back(Cell(j, i));
    }~Matrix() {}
    void draw(RenderWindow & window) {
        VertexArray square(LinesStrip, 4);
        square[0].color = Color::Black;
        square[1].color = Color::Black;
        square[2].color = Color::Black;
        square[3].color = Color::Black;
        for (auto & i: cells) {
            square[0].position = Vector2f(i.coordinates.first * csize, i.coordinates.second * csize);
            square[1].position = Vector2f(i.coordinates.first * csize + csize, i.coordinates.second * csize);
            square[2].position = Vector2f(i.coordinates.first * csize + csize, i.coordinates.second * csize + csize);
            square[3].position = Vector2f(i.coordinates.first * csize, i.coordinates.second * csize + csize);
            window.draw(square);
        }
    }
};

class Snake {
    public:
    list < Cell > cells;
    Cell head;
    Snake() {
        head = Cell(RNGrange(rng), RNGrange(rng));
        cells.push_back(head);
    }~Snake() {}
    void draw(RenderWindow & window) {
        VertexArray square(Quads, 4);
        square[0].color = Color::Green;
        square[1].color = Color::Green;
        square[2].color = Color::Green;
        square[3].color = Color::Green;
        for (auto & i: cells) {
            square[0].position = Vector2f(i.coordinates.first * csize, i.coordinates.second * csize);
            square[1].position = Vector2f(i.coordinates.first * csize + csize, i.coordinates.second * csize);
            square[2].position = Vector2f(i.coordinates.first * csize + csize, i.coordinates.second * csize + csize);
            square[3].position = Vector2f(i.coordinates.first * csize, i.coordinates.second * csize + csize);
            window.draw(square);
        }
    }
    void move(char c) {
        switch (c) {
        case 'W':
            head = Cell(head.coordinates.first, head.coordinates.second - 1);
            cells.push_front(head);
            cells.pop_back();
            break;
        case 'A':
            head = Cell(head.coordinates.first - 1, head.coordinates.second);
            cells.push_front(head);
            cells.pop_back();
            break;
        case 'S':
            head = Cell(head.coordinates.first, head.coordinates.second + 1);
            cells.push_front(head);
            cells.pop_back();
            break;
        case 'D':
            head = Cell(head.coordinates.first + 1, head.coordinates.second);
            cells.push_front(head);
            cells.pop_back();
            break;
        default:
            //yeeet
            break;
        }
    }
    bool searchCell(int x, int y) {
        for (auto & i: cells) {
            if (i.coordinates.first == x && i.coordinates.second == y)
                return true;
        }
        return false;
    }
};

class Apple {
    public:
    vector < Cell > cells;
    Apple() {
        cells.push_back(Cell(RNGrange(rng), RNGrange(rng)));
    }~Apple() {}
    void draw(RenderWindow & window) {
        VertexArray square(Quads, 4);
        square[0].color = Color::Red;
        square[1].color = Color::Red;
        square[2].color = Color::Red;
        square[3].color = Color::Red;
        for (auto & i: cells) {
            square[0].position = Vector2f(i.coordinates.first * csize, i.coordinates.second * csize);
            square[1].position = Vector2f(i.coordinates.first * csize + csize, i.coordinates.second * csize);
            square[2].position = Vector2f(i.coordinates.first * csize + csize, i.coordinates.second * csize + csize);
            square[3].position = Vector2f(i.coordinates.first * csize, i.coordinates.second * csize + csize);
            window.draw(square);
        }
    }
};

class Game {
    public:
    Matrix matrix;
    Snake snake;
    Apple apple;
    RenderWindow window;
    Event event;
    Font font;

    Game() {
        matrix = Matrix();
        snake = Snake();
        apple = Apple();
        window.create(VideoMode(msize * csize, msize * csize), "Jormungandr");
        window.setFramerateLimit(60);
        font.loadFromFile("Raleway-Regular.ttf");
    }~Game() {}
    void run() {
        Clock clock;
        Time elapsed;
        Music song;
        Music intro;
        Music outro;
        song.openFromFile("song.wav");
        intro.openFromFile("intro.wav");
        outro.openFromFile("outro.wav");
        intro.play();
        intro.setLoop(true);
        welcome();
        intro.stop();
        song.play();
        song.setLoop(true);
        window.clear(Color::White);
        snake.draw(window);
        apple.draw(window);
        matrix.draw(window);
        window.display();
        while (window.isOpen()) {
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                do {
                    while (window.pollEvent(event)) {
                        if (event.type == Event::Closed)
                            window.close();
                    }
                    elapsed = clock.getElapsedTime();
                    if (elapsed.asMilliseconds() > maxtime) {
                        if (snake.head.coordinates.second - 1 < 0) {
                            song.stop();
                            outro.play();
                            outro.setLoop(true);
                            gameOver();
                        } else {
                            if (snake.searchCell(snake.head.coordinates.first, snake.head.coordinates.second - 1)) {
                                snake.cells.resize(1);
                                snake.move('W');
                            } else {
                                if (snake.head.coordinates.first == apple.cells[0].coordinates.first && snake.head.coordinates.second - 1 == apple.cells[0].coordinates.second) {
                                    snake.cells.push_front(apple.cells[0]);
                                    apple = Apple();
                                    snake.move('W');
                                    if (snake.cells.size() >= win)
                                        shodan();

                                } else {
                                    snake.move('W');
                                }
                            }
                        }
                        window.clear(Color::White);
                        snake.draw(window);
                        apple.draw(window);
                        matrix.draw(window);
                        window.display();
                        clock.restart();
                    }
                } while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D));
            } else {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                    do {
                        while (window.pollEvent(event)) {
                            if (event.type == Event::Closed)
                                window.close();
                        }
                        elapsed = clock.getElapsedTime();
                        if (elapsed.asMilliseconds() > maxtime) {
                            if (snake.head.coordinates.first - 1 < 0) {
                                song.stop();
                                outro.play();
                                outro.setLoop(true);
                                gameOver();
                            } else {
                                if (snake.searchCell(snake.head.coordinates.first - 1, snake.head.coordinates.second)) {
                                    snake.cells.resize(1);
                                    snake.move('A');
                                } else {
                                    if (snake.head.coordinates.first - 1 == apple.cells[0].coordinates.first && snake.head.coordinates.second == apple.cells[0].coordinates.second) {
                                        snake.cells.push_front(apple.cells[0]);
                                        apple = Apple();
                                        snake.move('A');
                                        if (snake.cells.size() >= win)
                                            shodan();

                                    } else {
                                        snake.move('A');
                                    }
                                }
                            }
                            window.clear(Color::White);
                            snake.draw(window);
                            apple.draw(window);
                            matrix.draw(window);
                            window.display();
                            clock.restart();
                        }
                    } while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D));
                } else {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                        do {
                            while (window.pollEvent(event)) {
                                if (event.type == Event::Closed)
                                    window.close();
                            }
                            elapsed = clock.getElapsedTime();
                            if (elapsed.asMilliseconds() > maxtime) {
                                if (snake.head.coordinates.second + 1 >= msize) {
                                    song.stop();
                                    outro.play();
                                    outro.setLoop(true);
                                    gameOver();
                                } else {
                                    if (snake.searchCell(snake.head.coordinates.first, snake.head.coordinates.second + 1)) {
                                        snake.cells.resize(1);
                                        snake.move('S');
                                    } else {
                                        if (snake.head.coordinates.first == apple.cells[0].coordinates.first && snake.head.coordinates.second + 1 == apple.cells[0].coordinates.second) {
                                            snake.cells.push_front(apple.cells[0]);
                                            apple = Apple();
                                            snake.move('S');
                                            if (snake.cells.size() >= win)
                                                shodan();

                                        } else {
                                            snake.move('S');
                                        }
                                    }
                                }
                                window.clear(Color::White);
                                snake.draw(window);
                                apple.draw(window);
                                matrix.draw(window);
                                window.display();
                                clock.restart();
                            }
                        } while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A));
                    } else {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                            do {
                                while (window.pollEvent(event)) {
                                    if (event.type == Event::Closed)
                                        window.close();
                                }
                                elapsed = clock.getElapsedTime();
                                if (elapsed.asMilliseconds() > maxtime) {
                                    if (snake.head.coordinates.first + 1 >= msize) {
                                        song.stop();
                                        outro.play();
                                        outro.setLoop(true);
                                        gameOver();
                                    } else {
                                        if (snake.searchCell(snake.head.coordinates.first + 1, snake.head.coordinates.second)) {
                                            snake.cells.resize(1);
                                            snake.move('D');
                                        } else {
                                            if (snake.head.coordinates.first + 1 == apple.cells[0].coordinates.first && snake.head.coordinates.second == apple.cells[0].coordinates.second) {
                                                snake.cells.push_front(apple.cells[0]);
                                                apple = Apple();
                                                snake.move('D');
                                                if (snake.cells.size() >= win)
                                                    shodan();

                                            } else {
                                                snake.move('D');
                                            }
                                        }
                                    }
                                    window.clear(Color::White);
                                    snake.draw(window);
                                    apple.draw(window);
                                    matrix.draw(window);
                                    window.display();
                                    clock.restart();
                                }
                            } while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S));
                        } else {
                            //yeeet
                        }
                    }
                }
            }
        }
    }
    void gameOver() {
        Text text;

        window.clear(Color::White);
        text.setFont(font);
        text.setCharacterSize(32);
        text.setFillColor(Color::Black);
        text.setString("you lost :(");

        while (window.isOpen()) {
            while (window.pollEvent(event))
                if (event.type == Event::Closed)
                    window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                window.close();

            window.draw(text);
            window.display();
        }
    }
    void welcome() {
        Text text;

        window.clear(Color::White);
        text.setFont(font);
        text.setCharacterSize(32);
        text.setFillColor(Color::Black);
        text.setString("Jormungandr v2.2 by Vincent Press Enter to Continue");

        while (window.isOpen()) {
            while (window.pollEvent(event))
                if (event.type == Event::Closed)
                    window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                break;

            window.draw(text);
            window.display();
        }
    }
    void shodan() {
        Text text;

        window.clear(Color::White);
        text.setFont(font);
        text.setCharacterSize(32);
        text.setFillColor(Color::Black);
        text.setString("SHODAN");

        while (window.isOpen()) {
            while (window.pollEvent(event))
                if (event.type == Event::Closed)
                    window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                window.close();

            window.draw(text);
            window.display();
        }
    }
};

int main() {
    Game game;

    game.run();

    return 0;
}