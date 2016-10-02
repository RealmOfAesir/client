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

#include "texture.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "shader_utils.h"

using namespace std;

GLuint load_surface_into_opengl(string const & image) {
    SDL_Surface *surface = IMG_Load(image.c_str());
    if(!surface) {
        cout << "[texture] surface load failed: " << IMG_GetError() << endl;
        throw new runtime_error("[texture] surface load failed");
    }

    GLenum texture_format;
    GLenum internal_format;
    GLenum texture_type;

    if(surface->format->BytesPerPixel == 4) {
        cout << "[texture] 4 byte image" << endl;
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGBA;
            texture_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        } else {
            texture_format = GL_BGRA;
            texture_type = GL_UNSIGNED_INT_8_8_8_8;
            cout << "[texture] GL_BGRA" << endl;
        }
        internal_format = GL_RGBA8;
    } else if(surface->format->BytesPerPixel == 3) {
        cout << "[texture] 3 byte image " << endl;
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGB;
            texture_type = GL_UNSIGNED_BYTE;
        } else {
            texture_format = GL_BGR;
            texture_type = GL_UNSIGNED_BYTE;
            cout << "[texture] GL_BGR" << endl;
        }
        internal_format = GL_RGB8;
    } else {
        cout << "[texture] image " << image << " unknown BPP " << (int)surface->format->BytesPerPixel << endl;
        throw new runtime_error("[texture] unknown BPP");
    }

    cout << "[texture] size " << surface->w << "x" << surface->h << endl;

    int alignment = 8;
    cout << "[texture] surface->pitch " << surface->pitch << endl;
    while (surface->pitch % alignment) { // x%1==0 for any x
        alignment >>= 1;
    }
    cout << "[texture] alignment " << alignment << endl;
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, texture_format, texture_type, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(surface);

    return texture_id;
}

GLuint create_shader_program(string const & vertex_shader, string const & fragment_shader) {
    GLuint program_id = glCreateProgram();

    auto vertexShaderMaybe = load_shader_from_file(vertex_shader, GL_VERTEX_SHADER);
    if(!vertexShaderMaybe) {
        cout << "[texture] Couldn't load vertex shader file " << vertex_shader << endl;
        throw new runtime_error("[texture] Couldn't load vertex shader file");
    }

    glAttachShader(program_id, vertexShaderMaybe.value());

    auto fragmentShaderMaybe = load_shader_from_file(fragment_shader, GL_FRAGMENT_SHADER);
    if(!fragmentShaderMaybe) {
        cout << "[texture] Couldn't load fragment shader file " << fragment_shader << endl;
        throw new runtime_error("[texture] Couldn't load fragment shader file");
    }

    glAttachShader(program_id, fragmentShaderMaybe.value());

    glLinkProgram(program_id);

    glDeleteShader(vertexShaderMaybe.value());
    glDeleteShader(fragmentShaderMaybe.value());

    GLint programSucces = GL_TRUE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &programSucces);
    if(programSucces != GL_TRUE) {
        cout << "[texture] Couldn't link program " << program_id << endl;
        print_program_log(program_id);
        throw new runtime_error("[texture] Couldn't link program");
    }

    cout << "Created shader program " << program_id << endl;

    return program_id;
}

texture::texture(string const & image, string const & vertex_shader, string const & fragment_shader,
    glm::vec4 const position, glm::vec4 const clip) {

    _texture_id = load_surface_into_opengl(image);
    _program_id = create_shader_program(vertex_shader, fragment_shader);

    GLfloat vertexData[] = {
        -0.7f, -0.7f, 0.0f, 0.0f, 0.0f,
         0.7f, -0.7f, 0.0f, 1.0f, 0.0f,
        -0.7f,  0.7f, 0.0f, 0.0f, 1.0f,
         0.7f,  0.7f, 0.0f, 1.0f, 1.0f
    };

    glGenBuffers(1, &_buffer_object);
    glGenVertexArrays(1, &_vertex_array_id);
    glBindVertexArray(_vertex_array_id);

    glBindBuffer(GL_ARRAY_BUFFER, _buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
}

texture::~texture() {
    glDeleteBuffers(1, &_buffer_object);
    glDeleteProgram(_program_id);
    glDeleteTextures(1, &_texture_id);
}

void texture::render() {
    //cout << "[texture] rendering " << _program_id << " - " << _texture_id << " - " << _buffer_object << endl;
    glUseProgram(_program_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glUniform1i(glGetUniformLocation(_program_id, "textureUnit"), 0);

    glBindVertexArray(_vertex_array_id);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
}
