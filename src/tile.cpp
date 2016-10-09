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

#include "tile.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <easylogging++.h>

#include "shader_utils.h"
#include "texture_manager.h"

using namespace std;


// position = {x, y, x+w, y+h}
tile::tile(string const & image, string const & vertex_shader, string const & fragment_shader,
    glm::mat4 const projection_matrix, glm::vec4 const position, glm::vec4 const clip) : _image(image) {

    _texture_id = load_surface_into_opengl(image);
    _program_id = create_shader_program(vertex_shader, fragment_shader);
    _projection = projection_matrix;

    glm::mat4 model_matrix;

    model_matrix = glm::translate(model_matrix, glm::vec3(position.x, position.y, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(position.z, position.w, 1.0f));

    _model = model_matrix;

    GLfloat vertexData[] = {
         0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
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

    _projection_location = glGetUniformLocation(_program_id, "projection");
    if(_projection_location < 0) {
        LOG(FATAL) << "[tile] projection location not found in shader" << endl;
    }

    _model_location = glGetUniformLocation(_program_id, "model");
    if(_model_location < 0) {
        LOG(FATAL) << "[tile] model location not found in shader" << endl;
    }

    _textureunit_location = glGetUniformLocation(_program_id, "textureUnit");
    if(_textureunit_location < 0) {
        LOG(FATAL) << "[tile] textureUnit not found in shader" << endl;
    }
}

tile::~tile() {
    glDeleteBuffers(1, &_buffer_object);
    glDeleteProgram(_program_id);
    delete_texture(_image);
}

void tile::render() {
    //LOG(INFO) << "[tile] rendering " << _program_id << " - " << _texture_id << " - " << _buffer_object << endl;
    glUseProgram(_program_id);

    glUniformMatrix4fv(_projection_location, 1, GL_FALSE, glm::value_ptr(_projection));
    glUniformMatrix4fv(_model_location, 1, GL_FALSE, glm::value_ptr(_model));
    glUniform1i(_textureunit_location, 0);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glBindVertexArray(_vertex_array_id);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);

    glUseProgram(0);
}

void tile::set_projection(glm::mat4& projection) {
    _projection = projection;
    glUniformMatrix4fv(_projection_location, 1, GL_FALSE, glm::value_ptr(_projection));
}

void tile::set_model(glm::mat4& model) {
    _model = model;
    glUniformMatrix4fv(_model_location, 1, GL_FALSE, glm::value_ptr(model));
}
