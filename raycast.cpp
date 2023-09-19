#ifndef RAYCAST_CPP
#define RAYCAST_CPP

#include <math.h>

int worldMap[24][24] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

struct demo_state
{
    double posX = 22, posY = 12;        // x and y start position
    double dirX = -1, dirY = 0;         // initial direction vector
    double planeX = 0, planeY = 0.66;   // the 2d raycaster version of camera plane
    double moveSpeed = 0.016667f * 5.0; // the constant value is in squares/second
    double rotSpeed = 0.016667f * 3.0;  // the constant value is in radians/second
};

static demo_state demoState;

static void LodeVRaycastMove(bool up, bool down, bool left, bool right)
{
    demo_state result = demoState;
    if (up)
    {
        if (worldMap[int(result.posX + result.dirX * result.moveSpeed)][int(result.posY)] == false)
            result.posX += result.dirX * result.moveSpeed;
        if (worldMap[int(result.posX)][int(result.posY + result.dirY * result.moveSpeed)] == false)
            result.posY += result.dirY * result.moveSpeed;
    }
    if (down)
    {
        if (worldMap[int(result.posX - result.dirX * result.moveSpeed)][int(result.posY)] == false)
            result.posX -= result.dirX * result.moveSpeed;
        if (worldMap[int(result.posX)][int(result.posY - result.dirY * result.moveSpeed)] == false)
            result.posY -= result.dirY * result.moveSpeed;
    }
    if (right)
    {
        // both camera direction and camera plane must be rotated
        double oldDirX = result.dirX;
        result.dirX = result.dirX * cos(-result.rotSpeed) - result.dirY * sin(-result.rotSpeed);
        result.dirY = oldDirX * sin(-result.rotSpeed) + result.dirY * cos(-result.rotSpeed);
        double oldPlaneX = result.planeX;
        result.planeX = result.planeX * cos(-result.rotSpeed) - result.planeY * sin(-result.rotSpeed);
        result.planeY = oldPlaneX * sin(-result.rotSpeed) + result.planeY * cos(-result.rotSpeed);
    }
    if (left)
    {
        // both camera direction and camera plane must be rotated
        double oldDirX = result.dirX;
        result.dirX = result.dirX * cos(result.rotSpeed) - result.dirY * sin(result.rotSpeed);
        result.dirY = oldDirX * sin(result.rotSpeed) + result.dirY * cos(result.rotSpeed);
        double oldPlaneX = result.planeX;
        result.planeX = result.planeX * cos(result.rotSpeed) - result.planeY * sin(result.rotSpeed);
        result.planeY = oldPlaneX * sin(result.rotSpeed) + result.planeY * cos(result.rotSpeed);
    }
    demoState = result;
}

static void LodeVRaycast(int screenWidth, int screenHeight, unsigned int *screenData, int worldMap[24][24])
{
    demo_state state = demoState;
    for (int x = 0; x < screenWidth; x++)
    {
        // calculate ray position and direction
        double cameraX = 2 * x / double(screenWidth) - 1; // x-coordinate in camera space
        double rayDirX = state.dirX + state.planeX * cameraX;
        double rayDirY = state.dirY + state.planeY * cameraX;

        // which box of the map we're in
        int mapX = int(state.posX);
        int mapY = int(state.posY);

        // length of ray from current position to next x or y-side
        double sideDistX;
        double sideDistY;

        // length of ray from one x or y-side to next x or y-side
        double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
        double perpWallDist;

        // what direction to step in x or y-direction (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0; // was there a wall hit?
        int side;    // was a NS or a EW wall hit?

        // calculate step and initial sideDist
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (state.posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - state.posX) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (state.posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - state.posY) * deltaDistY;
        }

        // perform DDA
        while (hit == 0)
        {
            // jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            // Check if ray has hit a wall
            if (worldMap[mapX][mapY] > 0)
                hit = 1;
        }
        // Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
        if (side == 0)
            perpWallDist = (sideDistX - deltaDistX);
        else
            perpWallDist = (sideDistY - deltaDistY);

        // Calculate height of line to draw on screen
        int lineHeight = (int)(screenHeight / perpWallDist);

        // calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0)
            drawStart = 0;
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight)
            drawEnd = screenHeight - 1;

        // choose wall color
        unsigned int color;
        switch (worldMap[mapX][mapY])
        {
        case 1:
            color = 0xffff0000;
            break; // red
        case 2:
            color = 0xff00ff00;
            break; // green
        case 3:
            color = 0xff0000ff;
            break; // blue
        case 4:
            color = 0xffffffff;
            break; // white
        default:
            color = 0xffffff00;
            break; // yellow
        }

        // give x and y sides different brightness
        if (side == 1)
        {
            unsigned char r = ((color & 0x00ff0000) >> 16) / 2;
            unsigned char g = ((color & 0x0000ff00) >> 8) / 2;
            unsigned char b = (color & 0x000000ff) / 2;

            color = r << 16 | g << 8 | b;
        }

        // draw the pixels of the stripe as a vertical line
        for (int y = 0; y < drawEnd - drawStart; ++y)
        {
            int newY = y + (screenHeight - (drawEnd - drawStart)) / 2;
            screenData[x + newY * screenWidth] = color;
        }
        // end of lodeV raycast implementation
    }
}

#endif