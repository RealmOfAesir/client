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

#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 projection;

out vec2 texCoord;

void main()
{
    gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
    texCoord = vec2(inTexCoord.x, 1.0f - inTexCoord.y);
}
