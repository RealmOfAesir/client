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
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <easylogging++.h>

#include "shader_utils.h"
#include "timer.h"
#include "sprite.h"

using namespace std;

SDL_Window *window = nullptr;
SDL_GLContext context = nullptr;
vector<sprite*> sprites;
glm::mat4 projection;

constexpr int screenWidth = 1024;
constexpr int screenHeight = 768;

INITIALIZE_EASYLOGGINGPP

void initialize_logger() noexcept {
    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
    //defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureAllLoggers(defaultConf);
}

void init_sdl() noexcept {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG(ERROR) << "[main] SDL Init went wrong: " << SDL_GetError();
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow("Realm of Aesir", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(window == nullptr) {
        LOG(ERROR) << "[main] Couldn't initialize window: " << SDL_GetError();
        exit(1);
    }

    context = SDL_GL_CreateContext(window);
    if(context == nullptr) {
        LOG(ERROR) << "[main] Couldn't initialize context: " << SDL_GetError();
        exit(1);
    }

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK) {
        LOG(ERROR) << "[main] Error initializing GLEW! " << glewGetErrorString(glewError);
        exit(1);
	}

    if(SDL_GL_SetSwapInterval(1) < 0) {
        LOG(ERROR) << "[main] Couldn't initialize vsync: " << SDL_GetError();
        exit(1);
    }

    if(SDL_GL_MakeCurrent(window, context) < 0) {
        LOG(ERROR) << "[main] Couldn't make OpenGL context current: " << SDL_GetError();
        exit(1);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
}

void init_sdl_image() noexcept {
    int initted = IMG_Init(IMG_INIT_PNG);
    if((initted & IMG_INIT_PNG) != IMG_INIT_PNG) {
        LOG(ERROR) << "[main] SDL image init went wrong: " << IMG_GetError();
        exit(1);
    }
}

void close() noexcept
{
    for(auto *tex : sprites) {
        delete tex;
    }

	SDL_DestroyWindow(window);
	window = nullptr;

    IMG_Quit();

	SDL_Quit();
}

void render() noexcept
{
	glClear(GL_COLOR_BUFFER_BIT);

    for(auto& tex : sprites) {
        tex->render();
    }

    SDL_GL_SwapWindow(window);
}

void set_working_dir() noexcept {
    char *base_path = SDL_GetBasePath();
    if (base_path) {
        LOG(INFO) << "[main] Set base_path to " << base_path;
        chdir(base_path);
        SDL_free(base_path);
    } else {
        LOG(ERROR) << "[main] Couldn't get base path: " << SDL_GetError();
        exit(1);
    }

}

void init_extras() noexcept {
    ios::sync_with_stdio(false);
}

int main() {
    initialize_logger();
    init_sdl();
    set_working_dir();
    init_sdl_image();
    init_extras();

    bool quit = false;

    SDL_Event e;
    SDL_StartTextInput();

	timer fps_timer;
    int counted_frames = 0;

    for(int i = 0; i < 1; i++) {
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 10; y++) {
                sprites.push_back(new sprite("assets/tilesets/angband/dg_armor32.gif.png", "shaders/triangle_vertex.shader",
                    "shaders/triangle_fragment.shader", projection, glm::vec4(x * 32.0f, y * 32.0f, 32.0f, 32.0f), glm::vec4(x * 32.0f, y * 32.0f, 32.0f, 32.0f)));
            }
        }
    }

    sprites.push_back(new sprite("assets/tilesets/angband/dg_armor32.gif.png", "shaders/triangle_vertex.shader",
        "shaders/triangle_fragment.shader", projection, glm::vec4(320.0f, 320.0f, 320.0f, 320.0f), {}));

    fps_timer.start();

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

        if(fps_timer.get_ticks() > 1500) {
            LOG(INFO) << "[main] FPS: " << counted_frames / (fps_timer.get_ticks() / 1000.f);
            fps_timer.start();
            counted_frames = 0;
        }
    }

    SDL_StopTextInput();

    close();
    return 0;
}
