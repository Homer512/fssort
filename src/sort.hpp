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
#include "memberptr.hpp"
namespace fs {
  namespace internal {
    class FileSorterPrivate;
  }
  class FileSorter
  {
    OwningMemberPtr<internal::FileSorterPrivate> d;
    friend class internal::FileSorterPrivate;
    FileSorter(const FileSorter&);
    FileSorter& operator=(const FileSorter&);
  public:
    FileSorter(char line_end);
    ~FileSorter();
    void filter_stdin();
    void print_sorted();
  };
} // namespace fs
