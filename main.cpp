#include <iostream>
#include <vector>
#include <random>
// multi platform input handling
#if defined(_WIN32)
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

// width and height of room vector
constexpr int HEIGHT = 40;
constexpr int WIDTH  = 80;

class Player {
public:
    int x;
    int y;

    Player(int startX, int startY) : x(startX), y(startY) {}
};

class Engine {
private:
    std::string os_version = "unknown";
    bool run = true;
    std::vector<std::vector<char>> room;
    // rand setup
    std::mt19937 rng;
    std::bernoulli_distribution wallDist;
public:
    std::vector<std::vector<char>>& GetRoom() { return room; }
    const std::vector<std::vector<char>>& GetRoom() const { return room; }

    Engine(unsigned int seed = std::random_device{}()) : room(HEIGHT, std::vector<char>(WIDTH, ' ')), rng(seed), wallDist(0.1) { // (0.1) == 10% chance of wall
        #if defined(_WIN32)
            os_version = "windows";
        #elif defined(__APPLE__)
            os_version = "macos";
        #elif defined(__linux__)
            os_version = "linux";
        #else
            os_version = "unknown";
        #endif

        Generate();
    }

    void Generate() {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1) {
                    room[y][x] = '#';
                } else {
                    room[y][x] = wallDist(rng) ? '#' : ' ';
                }
            }
        }
    }

    void Render(const Player& player) const {
        std::cout << "\033[H\033[2J" << '\n';
        for (int y =  0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (x == player.x && y == player.y) {
                    std::cout << "\033[32m" << 'i' << "\033[0m"; // print green 'i' for player
                } else {
                    std::cout << room[y][x];
                }
            }
            std::cout << '\n';
        }
    }

    char getInput() {
        #if defined(_WIN32)
            return static_cast<char>(_getch());
        #else
            struct termios oldt, newt;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            int ch = getchar();
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return static_cast<char>(ch);
        #endif
    }
};

int main() {
    Engine engine;
    Player player(1, 1);
    char io;

    while(true) {
        engine.Render(player);

        io = engine.getInput();

        switch(io) {
            case 'w': if (player.y > 0 && engine.GetRoom()[player.y - 1][player.x] != '#') player.y--; break; // if player y greater than 0 and next move is not wall move up
            case 'a': if (player.x > 0 && engine.GetRoom()[player.y][player.x - 1] != '#') player.x--; break; // if player x greater than 0 and next move is not wall move left
            case 's': if (player.y < HEIGHT - 1 && engine.GetRoom()[player.y + 1][player.x] != '#') player.y++; break; // if player y less than HEIGHT - 1 and next move is not wall move down
            case 'd': if (player.x < WIDTH - 1 && engine.GetRoom()[player.y][player.x + 1] != '#') player.x++; break; // if player x less than WIDTH - 1 and next move is not wall move right
            case 'q': return 0;
            case 'r': engine.Generate(); break;
            default: break;
        }
    }
    
    return 0;
}