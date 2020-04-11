#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"





/*
TODO:

fps limiter / vsync
collision detection with all barriers?
Get color from bounce
color changer
configuration (clear all, FOV, etc)
remove barriers

Normals
Reflection
transparency

*/




const int startingPosX = 225;
const int startingPosY = 250;
const int rayLength = 2500;
const int rayAmount = 90;
const int barrierAmount = 1000;
const int rayInterval = 1;
const int screenWidth = 1200;
const int screenHeight = 800;
const olc::Pixel rayColour = olc::BLUE;
const olc::Pixel backgroundColor = olc::BLACK;
const olc::Pixel wallColor = olc::MAGENTA;
const olc::Pixel barrierColor = olc::WHITE;
int totalBarriers = 0;



bool withinMap(int x, int y)
{
    if ((x > 900 && x < screenWidth-1) && (y > 1 && y < 300)) return true;
    return false;
};



double calcDistance(int x1, int y1, int x2, int y2) // returns the distance between two points
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}


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


class Ray
{
public:
    double x, y;
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





int x = 1050;
int y = 150;
Ray rays[rayAmount];
Barrier barriers[barrierAmount];
Ray directionRay(x, y, ((rayAmount - rayInterval) / 2)* (M_PI / 180), olc::RED);







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
    




    bool creating = false;    // is true while button is held down
    Vec bVec1;
    Vec bVec2;



    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(backgroundColor);


        int mouseX = GetMouseX();
        int mouseY = GetMouseY();



        if (GetMouse(0).bPressed && (!creating && totalBarriers < barrierAmount) && withinMap(mouseX, mouseY))   // caches mouse position on mouse press
        {
            creating = true;
            bVec1 = Vec(mouseX, mouseY);
        }
        if (GetMouse(0).bReleased && creating && withinMap(mouseX, mouseY))   // caches mouse position on mouse release then creates barrier object
        {
            const int totBar = totalBarriers;
            creating = false;
            bVec2 = Vec(mouseX, mouseY);
            Barrier c(bVec1.x, bVec1.y, bVec2.x, bVec2.y, barrierColor);
            barriers[totBar] = c;
            totalBarriers++;
        }
        if (GetMouse(1).bReleased && totalBarriers > 6) {  // undo barrier creation
            const int totBar = --totalBarriers;
            barriers[totBar] = Barrier(0, 0, 0, 0, olc::BLANK);
        }




        if (GetKey(olc::W).bHeld) // Moves forward relative to direction ray
        {
            for (int i = 0; i < rayAmount; i++)
            {
                double directionVecX = directionRay.getDirVecX() * 1.15;
                double directionVecY = directionRay.getDirVecY() * 1.15;
                if (withinMap(rays[i].x + directionVecX, rays[i].y)) rays[i].x += directionVecX;  // check if within map seperatly so that if one returns false, the opposite can still move
                if (withinMap(rays[i].x, rays[i].y + directionVecY)) rays[i].y += directionVecY;
                if (withinMap(rays[i].x + directionVecX, rays[i].y) && i == 0) directionRay.x += directionRay.getDirVecX() * 1.15;
                if (withinMap(rays[i].x, rays[i].y + directionVecY) && i == 0) directionRay.y += directionRay.getDirVecY() * 1.15;
            }
        }



        if (GetKey(olc::A).bHeld) // rotates left
        {
            for (int i = 0; i < rayAmount; i++)
            {
                rays[i].angle -= 0.01;
            }
            directionRay.angle -= 0.01;
        }




        if (GetKey(olc::S).bHeld) // Moves backwards relative to direction ray
        {
            for (int i = 0; i < rayAmount; i++)
            {
                double directionVecX = directionRay.getDirVecX() * 1.15;
                double directionVecY = directionRay.getDirVecY() * 1.15;
                    if (withinMap(rays[i].x - directionVecX, rays[i].y)) rays[i].x -= directionVecX; // check if within map seperatly so that if one returns false, the opposite can still move
                    if (withinMap(rays[i].x, rays[i].y - directionVecY)) rays[i].y -= directionVecY;
                    if (withinMap(rays[i].x - directionVecX, rays[i].y) && i == 0) directionRay.x -= directionRay.getDirVecX() * 1.15;
                    if (withinMap(rays[i].x, rays[i].y - directionVecY) && i == 0) directionRay.y -= directionRay.getDirVecY() * 1.15;
            }
            
            

        }



        if (GetKey(olc::D).bHeld) // rotates right
        {
            for (int i = 0; i < rayAmount; i++)
            {
                rays[i].angle += 0.01;
            }
            directionRay.angle += 0.01;
        }












        for (int i = 0; i < rayAmount; i++) { // for each ray
            Ray ray = rays[i];
            int x1 = ray.x;
            int y1 = ray.y;
            double x2 = (ray.getDirVecX() * rayLength) + x1;
            double y2 = (ray.getDirVecY() * rayLength) + y1;
            double distance = (float) INFINITE;
            olc::Pixel color = ray.color;




            Vec intersections[barrierAmount];


            for (int i = 0; i < barrierAmount; i++) { // Checks for intersection
                Barrier barrier = barriers[i];
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
         //  FillCircle(x2, y2, 3, olc::GREEN); // Intersection points
        }


        for (int i = 0; i < barrierAmount; i++) {  // Draws barriers
            Barrier b = barriers[i];
            DrawLine(b.x1, b.y1, b.x2, b.y2, b.color);
        }



        DrawLine(directionRay.x, directionRay.y, directionRay.x + directionRay.getDirVecX() * 100, directionRay.y + directionRay.getDirVecY() * 100, directionRay.color);  // Draw direction line
        FillCircle(directionRay.x, directionRay.y, 4, olc::GREEN);  // Draw player on 2d Map


        return true;
    }
};











int main()
{
    Barrier top(0, 1, screenWidth - 1, 1, wallColor);
    Barrier map_bottom(900, 300, screenWidth, 300, wallColor);
    Barrier map_left(900, screenHeight, 900, 1, wallColor);
    Barrier right(screenWidth - 1, 0, screenWidth - 1, screenHeight, wallColor);
    Barrier left(1, 1, 1, screenHeight, wallColor);
    Barrier bottom(1, screenHeight-1, screenWidth-1, screenHeight-1, wallColor);
    barriers[0] = top;
    barriers[1] = map_bottom;
    barriers[2] = map_left;
    barriers[3] = right;
    barriers[4] = left;
    barriers[5] = bottom;
    totalBarriers += 6;



 





    for (int i = 0; i < rayAmount; i += rayInterval)  // creates rays
    {
        double radian = i * (M_PI / 180);
        Ray r(x, y, radian, rayColour);
        rays[i] = r;
    }


    RayCasting win;  
    if (win.Construct(screenWidth, screenHeight, 1, 1))
    {
        win.Start();
    }
}




