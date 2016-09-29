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
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include "shader_utils.h"
#include "timer.h"

using namespace std;

SDL_Window *window = nullptr;
SDL_GLContext context = nullptr;
GLuint programId = 0;
GLint vertexPos2DLocation = -1;
GLuint VAO = 0;
GLuint VBO = 0;
GLuint IBO = 0;
GLuint vertexArrayId = 0;

void init_gl() {
    SDL_GL_MakeCurrent(window, context);

    programId = glCreateProgram();

    auto vertexShaderMaybe = load_shader_from_file("shaders/triangle_vertex.shader", GL_VERTEX_SHADER);
    if(!vertexShaderMaybe) {
        exit(1);
    }

    glAttachShader(programId, vertexShaderMaybe.value());

    auto fragmentShaderMaybe = load_shader_from_file("shaders/triangle_fragment.shader", GL_FRAGMENT_SHADER);
    if(!fragmentShaderMaybe) {
        exit(1);
    }

    glAttachShader(programId, fragmentShaderMaybe.value());

    glLinkProgram(programId);

    glDeleteShader(vertexShaderMaybe.value());
    glDeleteShader(fragmentShaderMaybe.value());

    GLint programSucces = GL_TRUE;
    glGetProgramiv(programId, GL_LINK_STATUS, &programSucces);
    if(programSucces != GL_TRUE) {
        cout << "Could not link program " << programId << endl;
        print_program_log(programId);
        exit(1);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);

    GLfloat vertexData[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);
}

void init_sdl() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL Init went wrong: " << SDL_GetError() << endl;
        exit(1);
    }

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    window = SDL_CreateWindow("Realm of Aesir", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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
}

void close()
{
    glDeleteProgram(programId);

	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_Quit();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(programId);

    glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

	glUseProgram(0);
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
    init_gl();
    init_extras();

    bool quit = false;

    SDL_Event e;
    SDL_StartTextInput();

	timer fps_timer;
    int counted_frames = 0;
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
        SDL_GL_SwapWindow(window);
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
