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
Make more dynamic

Normals
Reflection
transparency
*/


/*
BUGS:

Direction ray can be offset
Fisheye effect due to using euclidean distance
Color isn't correctly altered based on distance
Bottom rect height for 3D projection
*/


/*
Enabled it to run on any pc by making it static


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
// contains distance values to be represented in "3D" 
double view[rayAmount]; 
float viewWidth = 900 / rayAmount;

bool withinMap(int x, int y)
{
    if ((x > 900 && x < screenWidth-1) && (y > 1 && y < 300)) return true;
    return false;
};


// returns the distance between two points
double calcDistance(int x1, int y1, int x2, int y2)
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
    



    // is true while button is held down
    bool creating = false;   
    Vec bVec1;
    Vec bVec2;



    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(backgroundColor);


        int mouseX = GetMouseX();
        int mouseY = GetMouseY();


        // caches mouse position on mouse press
        if (GetMouse(0).bPressed && (!creating && totalBarriers < barrierAmount) && withinMap(mouseX, mouseY))   
        {
            creating = true;
            bVec1 = Vec(mouseX, mouseY);
        }

        // caches mouse position on mouse release then creates barrier object
        if (GetMouse(0).bReleased && creating && withinMap(mouseX, mouseY))   
        {
            const int totBar = totalBarriers;
            creating = false;
            bVec2 = Vec(mouseX, mouseY);
            Barrier c(bVec1.x, bVec1.y, bVec2.x, bVec2.y, barrierColor);
            barriers[totBar] = c;
            totalBarriers++;
        }

        // undo barrier creation
        if (GetMouse(1).bReleased && totalBarriers > 6) {  
            const int totBar = --totalBarriers;
            barriers[totBar] = Barrier(0, 0, 0, 0, olc::BLANK);
        }



        // Moves forward relative to direction ray
        if (GetKey(olc::W).bHeld)
        {
            for (int i = 0; i < rayAmount; i++)
            {
                double directionVecX = directionRay.getDirVecX() * 1.15;
                double directionVecY = directionRay.getDirVecY() * 1.15;
                // check if within map seperatly so that if one returns false, the opposite can still move
                if (withinMap(rays[i].x + directionVecX, rays[i].y)) rays[i].x += directionVecX;  
                if (withinMap(rays[i].x, rays[i].y + directionVecY)) rays[i].y += directionVecY;
                if (withinMap(rays[i].x + directionVecX, rays[i].y) && i == 0) directionRay.x += directionRay.getDirVecX() * 1.15;
                if (withinMap(rays[i].x, rays[i].y + directionVecY) && i == 0) directionRay.y += directionRay.getDirVecY() * 1.15;
            }
        }


        // rotates left
        if (GetKey(olc::A).bHeld) 
        {
            for (int i = 0; i < rayAmount; i++)
            {
                rays[i].angle -= 0.01;
            }
            directionRay.angle -= 0.01;
        }



        // Moves backwards relative to direction ray
        if (GetKey(olc::S).bHeld) 
        {
            for (int i = 0; i < rayAmount; i++)
            {
                double directionVecX = directionRay.getDirVecX() * 1.15;
                double directionVecY = directionRay.getDirVecY() * 1.15;
                // check if within map seperatly so that if one returns false, the opposite can still move
                    if (withinMap(rays[i].x - directionVecX, rays[i].y)) rays[i].x -= directionVecX;
                    if (withinMap(rays[i].x, rays[i].y - directionVecY)) rays[i].y -= directionVecY;
                    if (withinMap(rays[i].x - directionVecX, rays[i].y) && i == 0) directionRay.x -= directionRay.getDirVecX() * 1.15;
                    if (withinMap(rays[i].x, rays[i].y - directionVecY) && i == 0) directionRay.y -= directionRay.getDirVecY() * 1.15;
            }
            
            

        }


        // rotates right
        if (GetKey(olc::D).bHeld)
        {
            for (int i = 0; i < rayAmount; i++)
            {
                rays[i].angle += 0.01;
            }
            directionRay.angle += 0.01;
        }











        // for each ray
        for (int i = 0; i < rayAmount; i++) {
            Ray ray = rays[i];
            int x1 = ray.x;
            int y1 = ray.y;
            double x2 = (ray.getDirVecX() * rayLength) + x1;
            double y2 = (ray.getDirVecY() * rayLength) + y1;
            double distance = (double) INFINITE;
            olc::Pixel color = ray.color;



            // contains each of the ray's intersection points
            Vec intersections[barrierAmount];   




            // Checks for intersection
            for (int i = 0; i < barrierAmount; i++) { 
                Barrier barrier = barriers[i];
                intersections[i] = barrier.checkIntersection((int)x1, (int)y1, (int)x2, (int)y2);
            }



            // Get closest point of intersection
            for (int i = 0; i < barrierAmount; i++) {  
                Vec obj = intersections[i];
                double newDistance = calcDistance(x1, y1, (int)obj.x, (int)obj.y);


                if ((obj.x != 0 && obj.y != 0) && newDistance < distance)
                {
                    distance = newDistance;
                    x2 = (double)obj.x;
                    y2 = (double)obj.y;
                }
            }


            // Draw ray
            DrawLine(x1, y1, (int)x2, (int)y2, color);  


            // Defines the distance from each intersection point within the view array
            view[i] = distance;


            // Intersection points
         //  FillCircle(x2, y2, 3, olc::GREEN); 
        }





        
        // Draws 3D representation
        for (int i = 0; i < rayAmount; i++) 
        {
            // least distance == 0
            // max distance == 300
            int height1 = 1;
            int height2 = screenHeight-1;
            double d = view[i];
            int amt = ((int) d / (300 / 255)) - 60;
            if (amt > 255) amt = 255;
            if (amt < 0) amt = 0;

            olc::Pixel color = olc::Pixel(255, 255, 255);
            color.r -= amt;
            color.g -= amt;
            color.b -= amt;
             
            height1 += amt;
            height2 -= amt;

            FillRect(i * viewWidth, height1, viewWidth, height2, color);
        }


        // Draws barriers
        for (int i = 0; i < barrierAmount; i++) {  
            Barrier b = barriers[i];
            DrawLine(b.x1, b.y1, b.x2, b.y2, b.color);
        }


        // Draw direction line
        DrawLine(directionRay.x, directionRay.y, directionRay.x + directionRay.getDirVecX() * 100, directionRay.y + directionRay.getDirVecY() * 100, directionRay.color);  
         // Draw player on 2d Map
        FillCircle(directionRay.x, directionRay.y, 4, olc::GREEN); 


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




