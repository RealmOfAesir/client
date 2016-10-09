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

#pragma once

#ifndef __gl_h_
#include <GL/glew.h>
#endif

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>

class texture {
    public:
        texture(std::string const & image, std::string const & vertex_shader, std::string const & fragment_shader,
            glm::mat4 const projection_matrix, glm::vec4 const position, glm::vec4 const clip);
        texture(texture&&) = default;

        ~texture();

        void render();
        void set_projection(glm::mat4& projection);
        void set_position(glm::mat4& position);
        void set_model(glm::mat4& model);

    private:
        GLuint _program_id;
        GLuint _buffer_object;
        GLuint _texture_id;
        GLuint _vertex_array_id;
        glm::vec4 _position;
        glm::vec4 _clip;
        glm::mat4 _projection;
        glm::mat4 _model;
        GLint _projection_location;
        GLint _model_location;
        GLint _textureunit_location;
};
