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
#include "sort.hpp"
#include "opts.hpp"
#include "verbose.hpp"
#include <cstdlib>
// using EXIT_SUCCESS, EXIT_FAILURE
#include <ios>
// using std::ios
#include <memory>
// using std::auto_ptr

int main(int argc, char** argv)
{
	fs::CmdOptions opts;
	opts.parse(argc, argv);
	if(! opts.is_valid())
		return EXIT_FAILURE;
	if(! opts.is_run())
		return EXIT_SUCCESS;
	/* After opt parsing we only use stdio for stderr and iostreams for cin,
	   cout. So syncing is unnecessary */
	std::ios::sync_with_stdio(false);
	char line_end = opts.is_zeroterm() ? '\0' : '\n';
	std::auto_ptr<fs::Notifier>
		notifier(fs::Notifier::create(opts.is_verbose()));
	fs::FileSorter sorter(line_end, notifier.get());
	sorter.filter_stdin();
	sorter.print_sorted();
	return EXIT_SUCCESS;
}
