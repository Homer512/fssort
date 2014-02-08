== FsSort ==

This is the development version of fssort, a tool to reorder filesystem
operations to reduce hard disk seek times.

fssort works by examining the physical block locations of a set of files
(which it reads from standard input). It sorts the files by increasing
block numbers and writes the reordered set to standard output.

At the moment, fssort works only on the second, third and fourth extended
filesystem (Ext2-4). It also typically requires root permissions to access
the filesystems directly.

The INSTALL file has instructions on building and installing fssort.
In case of bugs in this program, please contact Florian Philipp at
github@binarywings.net

FsSort is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
