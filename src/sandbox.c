#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define WINDOW_W 800
#define WINDOW_H 600
#define CELL_SIZE 8
#define GRID_W (WINDOW_W / CELL_SIZE)
#define GRID_H (WINDOW_H / CELL_SIZE)
#define TICK_MS 10
#define FIRE_LIFETIME 10
#define SMOKE_LIFETIME 100
#define STEAM_LIFETIME 200

/////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO
// pulzujici voda
//////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum Elements
{
    AIR,
    SAND,
    WATER,
    ACID,
    LAVA,
    STONE,
    WOOD,
    FIRE,
    SMOKE,
    STEAM,
    ASH,
    ELEMENTS_COUNT
} Elements;

typedef enum Types
{
    GAS,
    LIQUID,
    PARTICLE,
    SOLID
} Types;

char * elementNames[ELEMENTS_COUNT] = 
{
    "AIR",
    "SAND",
    "WATER",
    "ACID",
    "LAVA",
    "STONE",
    "WOOD",
    "FIRE",
    "SMOKE",
    "STEAM",
    "ASH"
};

int elementColors[ELEMENTS_COUNT][3] = 
{
    {0, 0, 0},             //air
    {255, 255, 0},         //sand
    {0, 0, 255},           //water
    {0, 255, 0},           //acid
    {255, 0, 0},           //lava
    {100, 100, 100},       //stone
    {80, 50, 50},          //wood
    {255, 128, 0},         //fire
    {200, 200, 200},       //smoke
    {150, 150, 200},       //steam
    {100, 100, 100},       //ash
};

typedef struct Cell
{
    int element;
    int type;
    bool updated;
    int lifetime;
    int color[3];
} Cell;

Cell cell_grid[GRID_W][GRID_H];

int getRandom(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

void createElement(int x, int y, int element)
{

    cell_grid[x][y].element = element;
    cell_grid[x][y].updated = false;
    cell_grid[x][y].lifetime = -1;

    if(cell_grid[x][y].element == SAND ||
        cell_grid[x][y].element == ASH)
    {
        cell_grid[x][y].type = PARTICLE;
    }
    if(cell_grid[x][y].element == WATER ||
        cell_grid[x][y].element == LAVA ||
        cell_grid[x][y].element == ACID)
    {
        cell_grid[x][y].type = LIQUID;
    }
    if(cell_grid[x][y].element == FIRE ||
        cell_grid[x][y].element == SMOKE ||
        cell_grid[x][y].element == STEAM ||
        cell_grid[x][y].element == AIR)
    {
        cell_grid[x][y].type = GAS;
    }
    if(cell_grid[x][y].element == STONE ||
            cell_grid[x][y].element == WOOD)
    {
        cell_grid[x][y].type = SOLID;
    }

    int colorShift = 30;
    for(int i = 0; i < 3; i++)
    {
        cell_grid[x][y].color[i] = elementColors[element][i];
        if(elementColors[element][i])
        {
            cell_grid[x][y].color[i] -= rand() % colorShift;
        }
    }
    
    if(element == FIRE)
        cell_grid[x][y].lifetime = FIRE_LIFETIME;
    if(element == SMOKE)
        cell_grid[x][y].lifetime = SMOKE_LIFETIME;
    if(element == STEAM)
        cell_grid[x][y].lifetime = STEAM_LIFETIME;
    if(element == LAVA)
        cell_grid[x][y].lifetime = 50;
}

void initGrid()
{
    for(int i = 0; i < GRID_W; i++)
    {
        for(int j = 0; j < GRID_H; j++)
        {
            createElement(i, j, AIR);
        }
    }
}

void switchCells(int x1, int y1, int x2, int y2)
{
    Cell temp = cell_grid[x1][y1];
    cell_grid[x1][y1] = cell_grid[x2][y2];
    cell_grid[x2][y2] = temp;
}

void updateParticle(int x, int y)
{
    if(cell_grid[x][y+1].type < PARTICLE &&
        y + 1 != GRID_H &&
        cell_grid[x][y].element != cell_grid[x][y+1].element)
    {
        switchCells(x, y, x, y+1);
    }

    else if(cell_grid[x-1][y+1].type < PARTICLE &&
        x != 0 &&
        y + 1 != GRID_H &&
        cell_grid[x][y].element != cell_grid[x-1][y+1].element)
    {
        switchCells(x, y, x-1, y+1);
    }

    else if(cell_grid[x+1][y+1].type < PARTICLE &&
        x + 1 != GRID_W &&
        y + 1 != GRID_H &&
        cell_grid[x][y].element != cell_grid[x+1][y+1].element)
    {
        switchCells(x, y, x+1, y+1);
    }

}

void updateLiquid(int x, int y)
{
    if(cell_grid[x][y].lifetime == 0)
    {
        switch(cell_grid[x][y].element)
        {
            case WATER:
                createElement(x, y, STEAM);
                break;
            case LAVA:
                createElement(x, y, STONE);
                break;
        }
        return;
    }

    if(cell_grid[x][y+1].type <= LIQUID &&
        y + 1 != GRID_H &&
        cell_grid[x][y].element != cell_grid[x][y+1].element)
    {
        switchCells(x, y, x, y+1);
    }

    else if(cell_grid[x-1][y+1].type <= LIQUID &&
        x != 0 &&
        y + 1 != GRID_H &&
        cell_grid[x][y].element != cell_grid[x-1][y+1].element)
    {
        switchCells(x, y, x-1, y+1);
    }

    else if(cell_grid[x+1][y+1].type <= LIQUID &&
        x + 1 != GRID_W &&
        y + 1 != GRID_H &&
        cell_grid[x][y].element != cell_grid[x+1][y+1].element)
    {
        switchCells(x, y, x+1, y+1);
    }

    else if(cell_grid[x-1][y].type <= LIQUID &&
        x != 0 &&
        cell_grid[x][y].element != cell_grid[x-1][y].element)
    {
        switchCells(x, y, x-1, y);
    }

    else if(cell_grid[x+1][y].type <= LIQUID &&
        x + 1 != GRID_W &&
        cell_grid[x][y].element != cell_grid[x+1][y].element)
    {
        switchCells(x, y, x+1, y);
    }

    if(cell_grid[x][y].element == WATER)
    {
        for(int i = - 1; i <= 1; i++)
        {
            for(int j = -1; j <= 1; j++)
            {
                if(cell_grid[x+i][y+j].element == FIRE || cell_grid[x+i][y+j].element == LAVA)
                {
                    cell_grid[x][y].lifetime = 5;
                }
            }
        }
    }
    if(cell_grid[x][y].element == ACID)
    {
        for(int i = - 1; i <= 1; i++)
        {
            for(int j = -1; j <= 1; j++)
            {
                if(cell_grid[x+i][y+j].type == SOLID || cell_grid[x+i][y+j].type == PARTICLE)
                {
                    cell_grid[x+i][y+j].lifetime = 5;
                }
            }
        }
    }
}

void updateGas(int x, int y)
{
    if(cell_grid[x][y].lifetime == 0)
    {
        switch(cell_grid[x][y].element)
        {
            case FIRE:
                if(rand() % 10 == 0) createElement(x, y, SMOKE);
                else createElement(x, y, AIR);
                break;
            case SMOKE:
                createElement(x, y, AIR);
                break;
            case STEAM:
                createElement(x, y, WATER);
                break;
        }
        return;
    }

    if(cell_grid[x][y].element == FIRE)
    {
        cell_grid[x][y].color[1] = 100 + 100 / cell_grid[x][y].lifetime;
        cell_grid[x][y].color[2] = 200 / cell_grid[x][y].lifetime;
    }

    int randDir = rand() % 3 - 1;
    if(cell_grid[x + randDir][y-1].type <= GAS &&
        y != 0 &&
        x + randDir != -1 &&
        x + randDir != GRID_W &&
        cell_grid[x][y].element != cell_grid[x + randDir][y-1].element)
    { 

        switchCells(x, y, x + randDir, y-1);
    }
    
    else if(cell_grid[x + randDir][y].type <= GAS &&
        x + randDir != -1 &&
        x + randDir != GRID_W &&
        cell_grid[x][y].element != cell_grid[x + randDir][y].element)
    {

        switchCells(x, y, x + randDir, y);
    }
}

void updateSolid(int x, int y)
{   
    if(cell_grid[x][y].element == WOOD)
    {
        for(int i = - 1; i <= 1; i++)
        {
            for(int j = -1; j <= 1; j++)
            {
                if(cell_grid[x+i][y+j].element == FIRE || cell_grid[x+i][y+j].element == LAVA)
                {
                    createElement(x, y, ASH);
                }
            }
        }
    }
}

int main()
{
    if(WINDOW_W % CELL_SIZE != 0 || WINDOW_H % CELL_SIZE != 0)
    {
        printf("non int grid size change cell size");
        return 1;
    }
    srand(time(0));
    SDL_Window* window = SDL_CreateWindow("Sandbox", WINDOW_W, WINDOW_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    initGrid(cell_grid);

    int selected_element = SAND;
    int brushSize = 2;

    bool running = true;
    Uint64 tickTimer = SDL_GetTicks();

    bool lmbdown = false;
    float mouseX;
    float mouseY;

    while(running)
    {

        printf("%i\n", cell_grid[0][0].type);

        if(SDL_GetTicks() - tickTimer < TICK_MS)
        {
            continue;
        }
        else
        {
            tickTimer = SDL_GetTicks();
        }

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if(event.key.scancode == SDL_SCANCODE_UP)
                {
                    if(selected_element == 0)
                    {
                        selected_element = ELEMENTS_COUNT - 1;
                    }
                    else
                    {
                        selected_element--;
                    }
                }
                if(event.key.scancode == SDL_SCANCODE_DOWN)
                {
                    if(selected_element == ELEMENTS_COUNT-1)
                    {
                        selected_element = 0;
                    }
                    else
                    {
                        selected_element++;
                    }
                }
                if(event.key.scancode == SDL_SCANCODE_LEFT)
                {
                    if(brushSize == 1)
                    {
                        brushSize = 10;
                    }
                    else
                    {
                        brushSize--;
                    }
                }
                if(event.key.scancode == SDL_SCANCODE_RIGHT)
                {
                    if(brushSize == 10)
                    {
                        brushSize = 1;
                    }
                    else
                    {
                        brushSize++;
                    }
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    lmbdown = true;
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    lmbdown = false;
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;
            }
        }

        //handle mouse buttons
        if(lmbdown)
        {
            int gridX = (mouseX / CELL_SIZE);
            int gridY = (mouseY / CELL_SIZE);


            for(int x = gridX; x < gridX + brushSize; x++)
            {
                for(int y = gridY; y < gridY + brushSize; y++)
                {
                    if(x >= 0 && x < GRID_W && y >= 0 && y < GRID_H) createElement(x, y, selected_element);
                }
            }
        }

        //update grid
        for(int x = 0; x < GRID_W; x++)
        {
            for(int y = 0; y < GRID_H; y++)
            {
                if(cell_grid[x][y].updated)
                    continue;

                cell_grid[x][y].updated = true;
                
                if(cell_grid[x][y].lifetime >= 0)
                    cell_grid[x][y].lifetime--;

                switch (cell_grid[x][y].type)
                {
                case GAS:
                    if(cell_grid[x][y].element != AIR) updateGas(x, y);
                    break;
                case LIQUID:
                    updateLiquid(x, y);
                    break;
                case PARTICLE:
                    updateParticle(x, y);
                    break;
                case SOLID:
                    updateSolid(x, y);
                    break;
                }
            }
        }

        //render grid

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_FRect cell_rect;
        for(int x = 0; x < GRID_W; ++x)
        {
            for(int y = 0; y < GRID_H; ++y)
            {

                cell_grid[x][y].updated = false;
                // switch(cell_grid[x][y].element)
                // {
                //     case FIRE:
                //         //int fireAlpha = 0 + cell_grid[w][h].lifetime * 5;
                //         SDL_SetRenderDrawColor(renderer, cell_grid[x][y].color[0], cell_grid[x][y].color[1], cell_grid[x][y].color[2], 50);
                //         break;
                //     default:
                //         SDL_SetRenderDrawColor(renderer, cell_grid[x][y].color[0], cell_grid[x][y].color[1], cell_grid[x][y].color[2], SDL_ALPHA_OPAQUE);
                //         break;
                // }

                SDL_SetRenderDrawColor(renderer, cell_grid[x][y].color[0], cell_grid[x][y].color[1], cell_grid[x][y].color[2], SDL_ALPHA_OPAQUE);

                cell_rect.w = CELL_SIZE;
                cell_rect.h = CELL_SIZE;
                cell_rect.x = x * CELL_SIZE;
                cell_rect.y = y * CELL_SIZE;
                SDL_RenderFillRect(renderer, &cell_rect);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderScale(renderer, 2, 2);
        SDL_RenderDebugTextFormat(renderer, WINDOW_W - 600, 0, "Selected element: %s", elementNames[selected_element]);
        SDL_RenderDebugTextFormat(renderer, 0, 0, "Brush size: %i", brushSize);
        SDL_SetRenderScale(renderer, 1, 1);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}