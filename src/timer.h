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

class timer
{
    public:
		timer();

		void start();
		void stop();
		void pause();
		void unpause();

		uint32_t get_ticks();

		bool is_started();
		bool is_paused();

    private:
		uint32_t _start_ticks;

		uint32_t _paused_ticks;

		bool _paused;
		bool _started;
};
