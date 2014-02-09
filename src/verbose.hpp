/*
 * Copyright Florian Philipp 2014
 *
 * This file is part of FsSort.
 *
 * FsSort is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FsSort is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FsSort.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _FS_VERBOSE_HPP
#define _FS_VERBOSE_HPP
namespace fs {
	class Notifier
	{
	public:
		virtual ~Notifier();
		virtual void log(const char* format, ...)
			__attribute__((format(printf, 2, 3))) = 0;
		virtual void ext_error(long err, const char* format, ...)
			__attribute__((format(printf, 3, 4))) = 0;
		virtual void errno_error(const char* format, ...)
			__attribute__((format(printf, 2, 3))) = 0;
		static Notifier* create(bool verbose);
	};
}
#endif /* _FS_VERBOSE_HPP */
