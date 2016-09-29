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

#include <GL/glew.h>

#include "custom_optional.h"

void print_shader_log(GLuint shader);
void print_program_log(GLuint program);
#ifdef EXPERIMENTAL_OPTIONAL
std::experimental::optional<GLuint> load_shader_from_file( std::string path, GLenum shaderType );
#else
std::optional<GLuint> load_shader_from_file( std::string path, GLenum shaderType );
#endif