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

#include <iostream>
#include <fstream>

#include "shader_utils.h"

using namespace std;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

void print_shader_log(GLuint shader) {
    if(glIsShader(shader)) {
        int infoLogLength = 0;
        int maxLength = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        char* infoLog = new char[maxLength];

        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0) {
            cout << infoLog << endl;
        }

        delete[] infoLog;
    } else {
        cout << "Name " << shader << " is not a shader" << endl;
    }
}

void print_program_log(GLuint program) {
    if(glIsProgram(program)) {
        int infoLogLength = 0;
        int maxLength = 0;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        char* infoLog = new char[maxLength];

        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0) {
            cout << infoLog << endl;
        }

        delete[] infoLog;
    } else {
        cout << "Name " << program << " is not a program" << endl;
    }
}

optional<GLuint> load_shader_from_file(string path, GLenum shaderType) {
    //Open file
    GLuint shaderID = 0;
    string shaderString;
    ifstream sourceFile(path);

    //Source file loaded
    if(!sourceFile) {
        cout << "Unable to open shader file " << path << endl;
        return {};
    }

    //Get shader source
    shaderString.assign((istreambuf_iterator<char>(sourceFile)), istreambuf_iterator<char>());

    shaderID = glCreateShader(shaderType);
    cout << "loading & compiling shader " << shaderID << " with path " << path << endl;

    const GLchar* shaderSource = shaderString.c_str();
    glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);

    glCompileShader(shaderID);

    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
    if(shaderCompiled != GL_TRUE) {
        cout << "Unable to compile shader " << shaderID << "\n\nSource: " << shaderSource << endl;
        print_shader_log(shaderID);
        glDeleteShader(shaderID);
        return {};
    }

    return make_optional(shaderID);
}
