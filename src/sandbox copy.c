#include <SDL3/SDL.h>

#include <stdio.h>

#define WINDOW_W 800
#define WINDOW_H 600
#define CELL_SIZE 5
#define GRID_W (WINDOW_W / CELL_SIZE)
#define GRID_H (WINDOW_H / CELL_SIZE)

// #define AIR 0
// #define SAND 1
// #define WATER 2
// #define STONE 3

typedef enum Elements
{
    AIR,
    SAND,
    WATER,
    LAVA,
    ACID,
    STONE,
    WOOD,
    FIRE,
    SMOKE,
    STEAM,
    RAINBOW
} Elements;

typedef struct
{
    int type;
    int next;
} Cell;

int main()
{
    SDL_Window* window = SDL_CreateWindow("Sandbox", WINDOW_W, WINDOW_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    Cell cell_grid[GRID_W][GRID_H] = {{0, 0}};

    int selected_element = SAND;
    int num_of_elements = sizeof(Elements);

    bool running = true;
    double dt = 0;
    float targetFrameTime = 5;
    Uint64 lastFrame = SDL_GetTicks();

    bool lmbdown = false;
    float mouseX;
    float mouseY;
    while(running)
    {
        
        if(SDL_GetTicks() - lastFrame < targetFrameTime)
        {
            continue;
        }
        else
        {
            lastFrame = SDL_GetTicks();
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
                        selected_element = num_of_elements - 1;
                    }
                    else
                    {
                        selected_element--;
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //handle mouse buttons
        if(lmbdown)
        {
            printf("left mouse button pressed\n");
            printf("mouse x = %f, mouse y = %f\n", event.button.x, event.button.y);
            int gridX = (mouseX / CELL_SIZE);
            int gridY = (mouseY / CELL_SIZE);
            switch (selected_element)
            {
            case SAND:
                cell_grid[gridX][gridY].type = SAND;
                break;
            case WATER:
                cell_grid[gridX][gridY].type = WATER;
                break;
            case STONE:
                cell_grid[gridX][gridY].type = STONE;
                break;
            }
        }

        //update grid
        for(int w = GRID_W - 1; w > 0; --w)
        {
            for(int h = GRID_H - 1; h > 0; --h)
            {
                switch (cell_grid[w][h].type) 
                {
                case SAND:
                    if(h+1 == GRID_H) continue;
                    if(cell_grid[w][h+1].type == AIR)
                    {
                        cell_grid[w][h+1].next = SAND;
                        cell_grid[w][h].next = AIR;
                    }
                    else if(cell_grid[w+1][h+1].type == AIR){
                        if(w+1 == GRID_W) continue;
                        cell_grid[w+1][h+1].next = SAND;
                        cell_grid[w][h].next = AIR;
                    }
                    else if(cell_grid[w-1][h+1].type == AIR){
                        if(w+1 == GRID_W) continue;
                        cell_grid[w-1][h+1].next = SAND;
                        cell_grid[w][h].next = AIR;
                    }
                    break;
                case WATER:
                    if(h+1 == GRID_H) continue;
                    if(cell_grid[w][h+1].type == AIR)
                    {
                        cell_grid[w][h+1].next = WATER;
                        cell_grid[w][h].next = AIR;
                    }
                    else if(cell_grid[w+1][h+1].type == AIR){
                        if(w+1 == GRID_W) continue;
                        cell_grid[w+1][h+1].next = WATER;
                        cell_grid[w][h].next = AIR;
                    }
                    else if(cell_grid[w-1][h+1].type == AIR){
                        if(w-1 == GRID_W) continue;
                        cell_grid[w-1][h+1].next = WATER;
                        cell_grid[w][h].next = AIR;
                    }
                    else if(cell_grid[w+1][h].type == AIR){
                        if(w+1 == GRID_W) continue;
                        cell_grid[w+1][h].next = WATER;
                        cell_grid[w][h].next = AIR;
                    }
                    else if(cell_grid[w-1][h].type == AIR){
                        if(w-1 == GRID_W) continue;
                        cell_grid[w-1][h].next = WATER;
                        cell_grid[w][h].next = AIR;
                    }
                    break;
                }
            }
        }


        //render grid
        SDL_FRect cell_rect;
        for(int w = 0; w < GRID_W; ++w)
        {
            for(int h = 0; h < GRID_H; ++h)
            {
                cell_grid[w][h].type = cell_grid[w][h].next; 
                switch (cell_grid[w][h].type)
                {
                case SAND:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    break;
                case WATER:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    break;
                case STONE:
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                }
                cell_rect.w = CELL_SIZE;
                cell_rect.h = CELL_SIZE;
                cell_rect.x = w * CELL_SIZE;
                cell_rect.y = h * CELL_SIZE;
                SDL_RenderFillRect(renderer, &cell_rect);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        switch(selected_element)
        {
        case AIR:
            SDL_RenderDebugTextFormat(renderer, WINDOW_W - 200, 0, "Selected element: AIR");
            break;
        case SAND:
            SDL_RenderDebugTextFormat(renderer, WINDOW_W - 200, 0, "Selected element: SAND");
            break;
        case WATER:
            SDL_RenderDebugTextFormat(renderer, WINDOW_W - 200, 0, "Selected element: WATER");
            break;
        case STONE:
            SDL_RenderDebugTextFormat(renderer, WINDOW_W - 200, 0, "Selected element: STONE");
            break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(5);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}