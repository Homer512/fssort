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
#include "verbose.hpp"
extern "C" {
# include <com_err.h>
// using ::com_err_va
# include <ext2fs/ext2_err.h>
// using ::initialize_ext2_error_table
}
#include <cstdio>
// using std::fprintf, std::vfprintf, ::stderr
#include <cstdarg>
// using std::va_list, va_start, va_end
#include <cerrno>
// using ::errno
#include <cstring>
// using std::strerror

namespace {
	class VerboseNotifier: public fs::Notifier
	{
	public:
		VerboseNotifier()
		{
			static bool err_init = false;
			if(! err_init) {
				err_init = true;
				::initialize_ext2_error_table();
			}
		}
		virtual ~VerboseNotifier()
		{}
		virtual void log(const char* format, ...)
		{
			std::fprintf(::stderr, "fssort: ");
			va_list args;
			va_start(args, format);
			std::vfprintf(::stderr, format, args);
			va_end(args);
			std::fprintf(::stderr, "\n");
		}
		virtual void ext_error(long err, const char* format, ...)
		{
			va_list args;
			va_start(args, format);
			::com_err_va("fssort", err, format, args);
			va_end(args);
		}
		virtual void errno_error(const char* format, ...)
		{
			std::fprintf(::stderr, "fssort: %s ", std::strerror(errno));
			va_list args;
			va_start(args, format);
			std::vfprintf(::stderr, format, args);
			va_end(args);
			std::fprintf(::stderr, "\n");
		}
	};
	class NormalNotifier: public fs::Notifier
	{
		virtual void log(const char* format, ...)
		{}
		virtual void ext_error(long err, const char* format, ...)
		{}
		virtual void errno_error(const char* format, ...)
		{}
	};
}

namespace fs {
	Notifier::~Notifier() {}
	Notifier* Notifier::create(bool verbose)
	{
		if(verbose)
			return new VerboseNotifier();
		else
			return new NormalNotifier();
	}
}
