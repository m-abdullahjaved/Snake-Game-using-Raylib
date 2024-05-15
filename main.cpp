#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <deque>
using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 20;
int cellCount = 22;
int offset = 40;

double lastTimeUpdated = 0;

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastTimeUpdated >= interval)
    {
        lastTimeUpdated = currentTime;
        return true;
    }
    return false;
}

bool elementsInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(element, deque[i]))
            return true;
    }
    return false;
}

class Snake
{
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 Direction = {1, 0};

public:
    bool addSegment = false;
    void Draw()
    {
        int size = body.size();
        for (int i = 0; i < size; i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            Rectangle rec = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(rec, 0.5, 5, darkGreen);
        }
    }
    void updatePosition()
    {
        body.push_front(Vector2Add(body[0], Direction));
        if (addSegment)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }
    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        Direction = {1, 0};
    }
    deque<Vector2> &getSnake()
    {
        return body;
    }
    Vector2 head()
    {
        return body[0];
    }
    void moveUpward()
    {
        if (Direction.y != 1)
            Direction = {0, -1};
    }
    void moveDownward()
    {
        if (Direction.y != -1)
            Direction = {0, 1};
    }
    void moveLeft()
    {
        if (Direction.x != 1)
            Direction = {-1, 0};
    }
    void moveRight()
    {
        if (Direction.x != -1)
            Direction = {1, 0};
    }
};

class Food
{
    Vector2 pos;
    Texture2D texture;

public:
    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        generateRandomPos(snakeBody);
    }
    ~Food()
    {
        UnloadTexture(texture);
    }
    Vector2 generateRandomCell()
    {
        float X = GetRandomValue(0, cellCount - 1);
        float Y = GetRandomValue(0, cellCount - 1);
        return {X, Y};
    }
    void generateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = generateRandomCell();
        while (elementsInDeque(position, snakeBody))
        {
            position = generateRandomCell();
        }
        pos = position;
    }
    Vector2 position()
    {
        return pos;
    }
    void Draw()
    {
        DrawTexture(texture, offset + pos.x * cellSize, offset + pos.y * cellSize, WHITE);
    }
};

class Game
{
    Snake snake;
    Food food;
    int score;
    Sound eat;
    Sound wall;

public:
    bool running;
    Game() : food(snake.getSnake())
    {
        InitAudioDevice();
        eat = LoadSound("eat.mp3");
        wall = LoadSound("wall.mp3");
        score = 0;
        running = true;
    }
    ~Game()
    {
        UnloadSound(eat);
        UnloadSound(wall);
        CloseAudioDevice();
    }
    void Draw()
    {
        food.Draw();
        snake.Draw();
    }
    Snake &getSnake()
    {
        return snake;
    }
    void Update()
    {
        if (running)
        {
            snake.updatePosition();
            checkCollisionwithFood();
            checkCollisionWithEdges();
            checkCollisionWithTail();
        }
    }
    void checkCollisionwithFood()
    {
        if (Vector2Equals(snake.head(), food.position()))
        {
            food.generateRandomPos(snake.getSnake());
            snake.addSegment = true;
            score++;
            PlaySound(eat);
        }
    }
    int getScore()
    {
        return score;
    }
    void checkCollisionWithEdges()
    {
        if (snake.getSnake()[0].x == cellCount || snake.getSnake()[0].x == -1)
            GameOver();
        if (snake.getSnake()[0].y == cellCount || snake.getSnake()[0].y == -1)
            GameOver();
    }
    void checkCollisionWithTail()
    {
        deque<Vector2> Tails = snake.getSnake();
        Vector2 head = Tails[0];
        Tails.pop_front();
        if (elementsInDeque(head, Tails))
        {
            GameOver();
        }
    }
    void GameOver()
    {
        snake.Reset();
        food.generateRandomPos(snake.getSnake());
        running = false;
        score = 0;
        PlaySound(wall);
    }
};

int main()
{

    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");
    SetTargetFPS(60);

    Game game;

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (eventTriggered(0.15))
            game.Update();

        if (IsKeyPressed(KEY_UP))
            game.getSnake().moveUpward();

        else if (IsKeyPressed(KEY_DOWN))
            game.getSnake().moveDownward();

        else if (IsKeyPressed(KEY_LEFT))
            game.getSnake().moveLeft();

        else if (IsKeyPressed(KEY_RIGHT))
            game.getSnake().moveRight();

        else if (IsKeyPressed(KEY_ENTER))
            game.running = true;

        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
        DrawText("Snake Game", offset + 10, 16, 20, darkGreen);
        DrawText(TextFormat("Score: %i", game.getScore()), cellSize * cellCount - offset, 16, 20, darkGreen);
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}