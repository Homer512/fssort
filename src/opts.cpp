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
#include "opts.hpp"
#include <unistd.h>
// using ::getopt, ::optopt
#include <cstdio>
// using std::puts, std::fprintf, ::stderr
#include <bitset>
// using std::bitset

namespace {
	void print_help()
	{
		std::puts("Usage: fssort [OPTION] ...\n"
				  "Write a file list sorted by physical location from "
				  "standard input to standard output.\n\n"
				  "Options:\n"
				  "\t-h\tprint this help\n"
				  "\t-v\tnotify about files that cannot be sorted\n"
				  "\t-V\tprint copyright and version information\n"
				  "\t-z\tend lines with 0 byte instead of newline");
	}
	void print_version()
	{
		std::puts("fssort version 0.1 Built "__DATE__"\n\n"
				  "Copyright 2014 Florian Philipp\n\n"
				  "This program comes with ABSOLUTELY NO WARRANTY;\n"
				  "for details, refer to the GNU General Public License\n"
				  "version 3: https://www.gnu.org/licenses/gpl-3.0.html\n\n"
				  "Report bugs and comments to github@binarywings.net");
	}
	void print_err(const char* arg)
	{
		std::fprintf(::stderr, "Unrecognized option '%s'\n"
					 "Try 'fssort -h' for more information\n",
					 arg);
	}
	void print_err()
	{
		std::fprintf(::stderr, "Unrecognized option -%c\n"
					 "Try 'fssort -h' for more information\n",
					 ::optopt);
	}
	void print_unexpected(int arg)
	{
		std::fprintf(::stderr, "Unexpected argument -%c\n"
					 "This is a bug. Please report it.\n"
					 "See 'fssort -V' for details.\n",
					 arg);
	}
	class GetOpt
	{
		int argc;
		char** argv;
		const char* format;
	public:
		GetOpt(int argc, char** argv, const char* format)
			: argc(argc),
			  argv(argv),
			  format(format)
		{}
		int operator()()
		{
			return ::getopt(argc, argv, format);
		}
	};
	enum {
		IS_VALID = 0, IS_RUN, IS_VERBOSE, IS_ZEROTERM,
		_OPTIONS_END
	};
} // namespace

namespace fs {
	namespace internal {
		struct CmdOptionsPrivate
		{
			typedef std::bitset<_OPTIONS_END> optset_t;
			optset_t opts;
			CmdOptionsPrivate()
				: opts(IS_VALID | IS_RUN)
			{}
		};
	} // namespace internal
	CmdOptions::CmdOptions()
		: d(new internal::CmdOptionsPrivate())
	{}
	CmdOptions::~CmdOptions()
	{}
	bool CmdOptions::is_valid() const
	{ return d->opts[IS_VALID]; }
	bool CmdOptions::is_run() const
	{ return d->opts[IS_RUN]; }
	bool CmdOptions::is_verbose() const
	{ return d->opts[IS_VERBOSE]; }
	bool CmdOptions::is_zeroterm() const
	{ return d->opts[IS_ZEROTERM]; }
	void CmdOptions::parse(int argc, char** argv)
	{
		internal::CmdOptionsPrivate::optset_t& opts = d->opts;
		GetOpt getopt(argc, argv, ":hvVz");
		for(int opt = getopt(); opts[IS_VALID] && opt != -1; opt = getopt()) {
			switch(opt) {
			case 'h':
				opts[IS_RUN] = false;
				print_help();
				break;
			case 'v':
				opts[IS_VERBOSE] = true;
				break;
			case 'V':
				opts[IS_RUN] = false;
				print_version();
				break;
			case 'z':
				opts[IS_ZEROTERM] = true;
				break;
			case '?':
				opts[IS_VALID] = false;
				print_err();
				break;
			default:
				opts[IS_VALID] = false;
				print_unexpected(opt);
				break;
			}
		}
		if(opts[IS_VALID] && ::optind < argc) {
			opts[IS_VALID] = false;
			print_err(argv[optind]);
		}
	}
} // namespace fs
