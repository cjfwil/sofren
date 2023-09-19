#ifndef RAYCAST_CPP
#define RAYCAST_CPP

#include <math.h>
#include <malloc.h>

int worldMap[64][64] =
    {
        {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7},
        {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
        {4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
        {4, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
        {4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
        {4, 0, 4, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 0, 7, 7, 7, 7, 7},
        {4, 0, 5, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
        {4, 0, 6, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
        {4, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 1},
        {4, 0, 8, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
        {4, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
        {4, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 1},
        {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
        {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
        {6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
        {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 6, 0, 6, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3},
        {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
        {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
        {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
        {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2},
        {4, 0, 0, 5, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
        {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
        {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
        {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3}};

double posX = 22.5, posY = 11.5;    // x and y start position
double dirX = -1, dirY = 0;         // initial direction vector
double planeX = 0, planeY = 0.66;   // the 2d raycaster version of camera plane
double moveSpeed = 0.016667f * 5.0; // the constant value is in squares/second
double rotSpeed = 0.016667f * 3.0;  // the constant value is in radians/second
int texWidth = 64, texHeight = 64;

struct LodeVDemo_texture
{
    unsigned int width = texWidth;
    unsigned int height = texHeight;
    unsigned int *data;
};

LodeVDemo_texture rcTextures[8];
bool rcTexturesLoaded = false;

static void LodeVRaycastMove(bool up, bool down, bool left, bool right)
{
    if (up)
    {
        if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false)
            posX += dirX * moveSpeed;
        if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false)
            posY += dirY * moveSpeed;
    }
    if (down)
    {
        if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false)
            posX -= dirX * moveSpeed;
        if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false)
            posY -= dirY * moveSpeed;
    }
    if (right)
    {
        // both camera direction and camera plane must be rotated
        double oldDirX = dirX;
        dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
        dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    if (left)
    {
        // both camera direction and camera plane must be rotated
        double oldDirX = dirX;
        dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
        dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
        planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }
}

static void LodeVRaycastSetup()
{
    for (int i = 0; i < 8; ++i)
    {
        rcTextures[i].data = (unsigned int *)malloc(rcTextures[i].width * rcTextures[i].height * sizeof(unsigned int));
    }
    // generate some textures
    for (int x = 0; x < texWidth; x++)
    {
        for (int y = 0; y < texHeight; y++)
        {
            int xorcolor = (x * 256 / texWidth) ^ (y * 256 / texHeight);
            // int xcolor = x * 256 / texWidth;
            int ycolor = y * 256 / texHeight;
            int xycolor = y * 128 / texHeight + x * 128 / texWidth;
            rcTextures[0].data[texWidth * y + x] = 65536 * 254 * (x != y && x != texWidth - y);  // flat red texture with black cross
            rcTextures[1].data[texWidth * y + x] = xycolor + 256 * xycolor + 65536 * xycolor;    // sloped greyscale
            rcTextures[2].data[texWidth * y + x] = 256 * xycolor + 65536 * xycolor;              // sloped yellow gradient
            rcTextures[3].data[texWidth * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor; // xor greyscale
            rcTextures[4].data[texWidth * y + x] = 256 * xorcolor;                               // xor green
            rcTextures[5].data[texWidth * y + x] = 65536 * 192 * (x % 16 && y % 16);             // red bricks
            rcTextures[6].data[texWidth * y + x] = 65536 * ycolor;                               // red gradient
            rcTextures[7].data[texWidth * y + x] = 128 + 256 * 128 + 65536 * 128;                // flat grey texture
        }
    }
    rcTexturesLoaded = true;
}

static void LodeVRaycastDemo(int screenWidth, int screenHeight, unsigned int *screenData)
{
    if (!rcTexturesLoaded) LodeVRaycastSetup();
    for (int x = 0; x < screenWidth; x++)
    {
        // calculate ray position and direction
        double cameraX = 2 * x / double(screenWidth) - 1; // x-coordinate in camera space
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        // which box of the map we're in
        int mapX = int(posX);
        int mapY = int(posY);

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
            sideDistX = (posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
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

        // texturing calculations
        int texNum = worldMap[mapX][mapY] - 1; // 1 subtracted from it so that texture 0 can be used!

        // calculate value of wallX
        double wallX; // where exactly the wall was hit
        if (side == 0)
            wallX = posY + perpWallDist * rayDirY;
        else
            wallX = posX + perpWallDist * rayDirX;
        wallX -= floor((wallX));

        // x coordinate on the texture
        int texX = int(wallX * double(texWidth));
        if (side == 0 && rayDirX > 0)
            texX = texWidth - texX - 1;
        if (side == 1 && rayDirY < 0)
            texX = texWidth - texX - 1;

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

        // How much to increase the texture coordinate per screen pixel
        double step = 1.0 * texHeight / lineHeight;
        // Starting texture coordinate
        double texPos = (drawStart - screenHeight / 2 + lineHeight / 2) * step;
        for (int y = drawStart; y < drawEnd; y++)
        {
            // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
            int texY = (int)texPos & (texHeight - 1);
            texPos += step;
            color = rcTextures[texNum].data[texHeight * texY + texX];
            // make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
            if (side == 1)
                color = (color >> 1) & 8355711;
            screenData[x + y * screenWidth] = color;
        }

        // // draw the pixels of the stripe as a vertical line
        // for (int y = drawStart; y < drawEnd; ++y)
        // {
        //     screenData[x + y * screenWidth] = color;
        // }
        // end of lodeV raycast implementation
    }
}

#endif
