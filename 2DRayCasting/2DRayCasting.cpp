#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"





/*
TODO:

Normals
Reflection
Get color from bounce
transparency
*/










const int startingPosX = 225;
const int startingPosY = 250;
const int rayLength = 2500;
const int rayAmount = 360;
const int barrierAmount = 6;
const int rayInterval = 1;
const int screenWidth = 1200;
const int screenHeight = 800;
const olc::Pixel rayColour = olc::BLUE;
const olc::Pixel backgroundColor = olc::BLACK;
const olc::Pixel wallColor = olc::MAGENTA;

double calcDistance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}




class Ray
{
public:
    int x, y;
    double angle;
    olc::Pixel color;

    Ray()
    {
        x = 0;
        y = 0;
        angle = 0;
        color = olc::WHITE;
    }
    Ray(int posX, int posY, double a, olc::Pixel c)
    {
        x = posX;
        y = posY;
        angle = a;
        color = c;
    }

    double getDirVecX()
    {
        return cos(angle);
    }
    double getDirVecY()
    {
        return sin(angle);
    }

};

class Vec
{
public:
    float x;
    float y;
    Vec()
    {
        x = 0;
        y = 0;
    }
    Vec(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
};

class Barrier
{
public:
    int x1;
    int y1;
    int x2;
    int y2;
    olc::Pixel color;

    Barrier()
    {
        x1 = 0;
        y1 = 0;
        x2 = 0;
        y2 = 0;
        color = olc::RED;
    }
    Barrier(int _x1, int _y1, int _x2, int _y2, olc::Pixel c)
    {
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
        color = c;
    }
    Vec checkIntersection(int x3, int y3, int x4, int y4)
    {
        float den = (float)(x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

        if (den == 0) return Vec();
        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
        float x = x1 + t * (x2 - x1);
        float y = y1 + t * (y2 - y1);
        return (t > 0 && t < 1 && u > 0) ? Vec(x, y) : Vec();
    }

};

class World
{
public:
    Ray rays[rayAmount];
    Barrier barriers[barrierAmount];
};






World world = World();








class RayCasting : public olc::PixelGameEngine
{
public:
    RayCasting()
    {
        sAppName = "RayCasting";
    }
public:
    bool OnUserCreate() override
    {
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(backgroundColor);
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();



        for (int i = 0; i < rayAmount; i += rayInterval)
        {
            double radian = i * (M_PI / 180);
            Ray r(mouseX, mouseY, radian, rayColour);
            world.rays[i] = r;
        }


        for (int i = 0; i < rayAmount; i++) { // for each ray
            Ray ray = world.rays[i];
            int x1 = ray.x;
            int y1 = ray.y;
            double x2 = (ray.getDirVecX() * rayLength) + x1;
            double y2 = (ray.getDirVecY() * rayLength) + y1;
            double distance = (float)INFINITE;
            olc::Pixel color = ray.color;




            Vec intersections[barrierAmount];


            for (int i = 0; i < barrierAmount; i++) { // Checks for intersection
                Barrier barrier = world.barriers[i];
                intersections[i] = barrier.checkIntersection((int)x1, (int)y1, (int)x2, (int)y2);
            }

            for (int i = 0; i < barrierAmount; i++) {  // Get closest point of intersection
                Vec obj = intersections[i];
                double newDistance = calcDistance(x1, y1, (int)obj.x, (int)obj.y);


                if ((obj.x != 0 && obj.y != 0) && newDistance < distance)
                {
                    distance = newDistance;
                    x2 = (double)obj.x;
                    y2 = (double)obj.y;
                }
            }

            DrawLine(x1, y1, (int)x2, (int)y2, color);  // Draw ray
           //FillCircle(x2, y2, 3, olc::GREEN); // Intersection points
        }


        for (int i = 0; i < barrierAmount; i++) {  // Draws barriers
            Barrier b = world.barriers[i];
            DrawLine(b.x1, b.y1, b.x2, b.y2, b.color);
        }


        return true;
    }
};












int main()
{
    Barrier top(0, 1, screenWidth - 1, 1, wallColor);
    world.barriers[0] = top;
    Barrier bottom(0, screenHeight - 1, screenWidth, screenHeight - 1, wallColor);
    world.barriers[1] = bottom;
    Barrier left(1, 0, 1, screenHeight - 1, wallColor);
    world.barriers[2] = left;
    Barrier right(screenWidth - 1, 0, screenWidth - 1, 10000, wallColor);
    world.barriers[3] = right;



    Barrier b(200, 500, 400, 600, olc::MAGENTA);
    world.barriers[4] = b;
    Barrier c(300, 600, 500, 700, olc::MAGENTA);
    world.barriers[5] = c;

    RayCasting win;
    if (win.Construct(screenWidth, screenHeight, 1, 1))
    {
        win.Start();
    }
}




