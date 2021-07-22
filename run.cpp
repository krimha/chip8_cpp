#include <SDL2/SDL.h>
#include <iostream>
#include <unordered_map>

int main(int argc, char* argv[])
{
        SDL_Window* window;
        SDL_Renderer* renderer;

        // Initialize SDL.
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
                return 1;

        // Create the window where we will draw.
        window = SDL_CreateWindow("SDL_RenderClear",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        512, 512,
                        0);

        // We must call SDL_CreateRenderer in order for draw calls to affect this window.
        renderer = SDL_CreateRenderer(window, -1, 0);

        // Select the color for drawing. It is set to red here.
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Clear the entire screen to our selected color.
        SDL_RenderClear(renderer);

        // Up until now everything was drawn behind the scenes.
        // This will show the new, red contents of the window.
        SDL_RenderPresent(renderer);


        std::unordered_map<int,char> scan_map = {
            {SDL_SCANCODE_1, '1'}, 
            {SDL_SCANCODE_2, '2'},
            {SDL_SCANCODE_3, '3'},
            {SDL_SCANCODE_4, 'C'},
            {SDL_SCANCODE_Q, '4'},
            {SDL_SCANCODE_W, '5'},
            {SDL_SCANCODE_E, '6'},
            {SDL_SCANCODE_R, 'D'},
            {SDL_SCANCODE_A, '7'},
            {SDL_SCANCODE_S, '8'},
            {SDL_SCANCODE_D, '9'},
            {SDL_SCANCODE_F, 'E'},
            {SDL_SCANCODE_Z, 'A'},
            {SDL_SCANCODE_X, '0'},
            {SDL_SCANCODE_C, 'B'},
            {SDL_SCANCODE_V, 'F'}
        };


        bool closed = false;
        while (!closed) {
            SDL_Event event;

            while (SDL_PollEvent(&event)) {
                switch (event.type) {

                case SDL_QUIT:
                    closed = true;
                    break;
                
                case SDL_KEYDOWN:
                    auto sc_code = event.key.keysym.scancode;
                    if (scan_map.find(sc_code) != scan_map.end())
                        std::cout << scan_map[sc_code] << '\n';
                    else
                        std::cout << "Not recognize\n";
                }
            }
        }


        // Always be sure to clean up
        SDL_Quit();
        return 0;
}
