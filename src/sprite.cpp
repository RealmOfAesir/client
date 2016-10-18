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

#include "sprite.h"

#include <array>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <easylogging++.h>

#include "shader_utils.h"
#include "texture_manager.h"

using namespace std;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

sprite::sprite(string const & image, string const & vertex_shader, string const & fragment_shader,
    glm::mat4 const projection_matrix, glm::vec4 const position, optional<glm::vec4> const clip) noexcept
     : _image(image), _texture(create_texture_from_image(image)) {

    _program_id = create_shader_program(vertex_shader, fragment_shader);
    _projection = projection_matrix;

    array<GLfloat, 16> vertexData;

    float x = position.x;
    float y = position.y;
    float w = position.z;
    float h = position.w;

    vertexData = {
        x, y, 0.0f, 0.0f,
        x+w, y, 1.0f, 0.0f,
        x, y+h, 0.0f, 1.0f,
        x+w, y+h, 1.0f, 1.0f
    };

    if(clip) {
        if(clip.value().x < 0 || clip.value().x > _texture._width || clip.value().y < 0 || clip.value().y > _texture._height) {
            LOG(FATAL) << "clip out of bounds";
        }
        
        vertexData[2] = clip.value().x / _texture._width;
        vertexData[3] = clip.value().y / _texture._height;

        vertexData[6] = (clip.value().x + clip.value().z) / _texture._width;
        vertexData[7] = clip.value().y / _texture._height;

        vertexData[10] = clip.value().x / _texture._width;
        vertexData[11] = (clip.value().y + clip.value().w) / _texture._height;

        vertexData[14] = (clip.value().x + clip.value().z) / _texture._width;
        vertexData[15] = (clip.value().y + clip.value().w) / _texture._height;
    }

    glGenBuffers(1, &_buffer_object);
    glGenVertexArrays(1, &_vertex_array_id);
    glBindVertexArray(_vertex_array_id);

    glBindBuffer(GL_ARRAY_BUFFER, _buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindVertexArray(0);

    glUseProgram(_program_id);

    _projection_location = glGetUniformLocation(_program_id, "projection");
    if(_projection_location < 0) {
        LOG(FATAL) << "[tile] projection location not found in shader" << endl;
    }
    glUniformMatrix4fv(_projection_location, 1, GL_FALSE, glm::value_ptr(_projection));

    _textureunit_location = glGetUniformLocation(_program_id, "textureUnit");
    if(_textureunit_location < 0) {
        LOG(FATAL) << "[tile] textureUnit not found in shader" << endl;
    }
    glUniform1i(_textureunit_location, GL_TEXTURE0);

    glUseProgram(0);
}

sprite::~sprite() noexcept {
    glDeleteBuffers(1, &_buffer_object);
    glDeleteProgram(_program_id);
    delete_texture(_image);
}

void sprite::render() const noexcept {
    //LOG(INFO) << "[tile] rendering " << _program_id << " - " << _texture_id << " - " << _buffer_object << endl;
    glUseProgram(_program_id);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, _texture._texture_id);
    glBindVertexArray(_vertex_array_id);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);

    glUseProgram(0);
}

void sprite::set_projection(glm::mat4& projection) noexcept {
    _projection = projection;
    glUniformMatrix4fv(_projection_location, 1, GL_FALSE, glm::value_ptr(_projection));
}