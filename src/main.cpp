/*
    Realm of Aesir client
    Copyright (C) 2016  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <glm/vec4.hpp>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include "custom_optional.h"
#include "shader_utils.h"
#include "timer.h"
#include "texture.h"

using namespace std;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

SDL_Window *window = nullptr;
SDL_GLContext context = nullptr;
optional<texture*> _texture = {};

void init_sdl() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL Init went wrong: " << SDL_GetError() << endl;
        exit(1);
    }

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    window = SDL_CreateWindow("Realm of Aesir", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(window == nullptr) {
        cout << "Couldn't initialize window: " << SDL_GetError() << endl;
        exit(1);
    }

    context = SDL_GL_CreateContext(window);
    if(context == nullptr) {
        cout << "Couldn't initialize context: " << SDL_GetError() << endl;
        exit(1);
    }

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK) {
        cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << endl;
        exit(1);
	}

    if(SDL_GL_SetSwapInterval(1) < 0) {
        cout << "Couldn't initialize vsync: " << SDL_GetError() << endl;
        exit(1);
    }

    if(SDL_GL_MakeCurrent(window, context) < 0) {
        cout << "Couldn't make OpenGL context current: " << SDL_GetError() << endl;
        exit(1);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void init_sdl_image() {
    int initted = IMG_Init(IMG_INIT_PNG);
    if((initted & IMG_INIT_PNG) != IMG_INIT_PNG) {
        cout << "SDL image init went wrong: " << IMG_GetError() << endl;
        exit(1);
    }
}

void close()
{
    if(_texture) {
        delete _texture.value();
    }

	SDL_DestroyWindow(window);
	window = nullptr;

    IMG_Quit();

	SDL_Quit();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

    if(_texture) {
        _texture.value()->render();
    }

    SDL_GL_SwapWindow(window);
}

void set_working_dir() {
    char *base_path = SDL_GetBasePath();
    if (base_path) {
        cout << "Set base_path to " << base_path << endl;
        chdir(base_path);
        SDL_free(base_path);
    } else {
        cout << "Couldn't get base path: " << SDL_GetError() << endl;
        exit(1);
    }

}

void init_extras() {
    ios::sync_with_stdio(false);
}

int main() {
    init_sdl();
    set_working_dir();
    init_sdl_image();
    init_extras();

    bool quit = false;

    SDL_Event e;
    SDL_StartTextInput();

	timer fps_timer;
    int counted_frames = 0;
    fps_timer.start();

    _texture = make_optional(new texture("assets/tilesets/angband/dg_armor32.gif.png", "shaders/triangle_vertex.shader",
        "shaders/triangle_fragment.shader", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)));

    while(!quit) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            } else if(e.type == SDL_TEXTINPUT) {
                int x = 0;
                int y = 0;
                SDL_GetMouseState(&x, &y);
            }
        }

        render();

        ++counted_frames;

        if(counted_frames > 0 && counted_frames % 180 == 0) {
            cout << "FPS: " << counted_frames / (fps_timer.get_ticks() / 1000.f) << endl;
        }

        if(counted_frames > 4'000'000'000) {
            counted_frames = 0;
        }
    }

    SDL_StopTextInput();

    close();
    return 0;
}
