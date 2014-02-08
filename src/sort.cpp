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
#define _BSD_SOURCE
// using major, minor from types.h
#include "sort.hpp"
#include <ext2fs/ext2fs.h>
// using ext2_*
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// using stat
#include <cstdio>
// using std::snprintf
#include <cstdlib>
// using std::realloc, std::free
#include <map>
// using std::multimap
#include <utility>
// using std::pair, std::make_pair
#include <string>
// using std::string, std::getline
#include <iostream>
// using std::cin, std::cout

namespace {
  typedef std::multimap<blk64_t, std::pair<std::string, blk64_t> > blockmap_t;
  struct BlockIter
  {
    blk64_t first, last;
    BlockIter(): first(0), last(0) {}
    static int iter(ext2_filsys fs, blk64_t* blocknr, e2_blkcnt_t blockcnt,
		    blk64_t ref_blk, int ref_offset, void* priv_data)
    {
      BlockIter* self = static_cast<BlockIter*>(priv_data);
      if(self->first == 0)
	self->first = *blocknr;
      self->last = *blocknr;
      return 0;
    }
  };
  class FileScanner
  {
    class FsState
    {
    protected:
      FsState() {}
      ~FsState() {}
    public:
      virtual ext2_filsys open(FileScanner& parent, dev_t dev);
    };
    struct UninitFsState: FsState
    {};
    class OpenFsState: public FsState
    {
      ext2_filsys fs;
    public:
      OpenFsState(): fs(NULL) {}
      ~OpenFsState()
      {
	if(fs)
	  ext2fs_close(fs);
      }
      void set_dev(ext2_filsys fs)
      { this->fs = fs; }
      virtual ext2_filsys open(FileScanner& parent, dev_t dev);
    };
    struct InvalidFsState: public FsState
    {
      virtual ext2_filsys open(FileScanner& parent, dev_t dev);
    };
    blockmap_t& blockmap;
    char* blockbuf;
    FsState* fs_state;
    dev_t cur_dev;
    OpenFsState open_state;
    InvalidFsState invalid_state;
    UninitFsState uninit_state;
    FileScanner(const FileScanner&);
    FileScanner& operator=(const FileScanner&);
  public:
    FileScanner(blockmap_t& blockmap)
      : blockmap(blockmap),
	blockbuf(NULL),
	fs_state(&uninit_state)
    {}
    ~FileScanner()
    {
      std::free(blockbuf);
    }
    bool operator()(const std::string& fname)
    {
      struct stat s;
      if(stat(fname.c_str(), &s))
	return true;
      ino_t inode = s.st_ino;
      dev_t device = s.st_dev;
      ext2_filsys fs = fs_state->open(*this, device);
      if(fs == NULL)
	return true;
      blockbuf = static_cast<char*>(std::realloc(blockbuf, 3 * s.st_blksize));
      BlockIter blk;
      if(ext2fs_block_iterate3(fs, inode, BLOCK_FLAG_DATA_ONLY
			       | BLOCK_FLAG_READ_ONLY, blockbuf,
			       &BlockIter::iter, &blk))
	return true;
      blockmap_t::mapped_type m(fname, blk.last);
      blockmap_t::value_type v(blk.first, m);
      blockmap.insert(v);
      return false;
    }
  };
  ext2_filsys FileScanner::FsState::open(FileScanner& parent, dev_t dev)
  {
    parent.cur_dev = dev;
    unsigned maj = major(dev);
    unsigned min = minor(dev);
    char devname[11 /*/dev/block/*/ + 8 /*255:255\0*/];
    std::snprintf(devname, sizeof(devname), "/dev/block/%u:%u", maj, min);
    ext2_filsys fs;
    if(ext2fs_open(devname, EXT2_FLAG_64BITS, 0 /*superblock*/,
		   0 /*blk size*/, unix_io_manager, &fs)) {
      parent.fs_state = &parent.invalid_state;
      fs = NULL;
    }
    else {
      parent.open_state.set_dev(fs);
      parent.fs_state = &parent.open_state;
    }
    return fs;
  }
  ext2_filsys FileScanner::InvalidFsState::open(FileScanner& parent, dev_t dev)
  {
    if(dev == parent.cur_dev)
      return NULL;
    else
      return this->FsState::open(parent, dev);
  }
 ext2_filsys FileScanner::OpenFsState::open(FileScanner& parent, dev_t dev)
 {
   if(dev == parent.cur_dev)
     return fs;
   else {
     ext2fs_close(fs);
     fs = NULL;
     return this->FsState::open(parent, dev);
   }
 }
} // namespace
namespace fs {
  namespace internal {
    struct FileSorterPrivate
    {
      blockmap_t blockmap;
      char line_end;
      FileSorterPrivate(char line_end)
	: line_end(line_end)
      {}
      void write(const std::string& str)
      {
	(std::cout << str).put(line_end);
      }
    };
  } // namespace internal

  FileSorter::FileSorter(char line_end)
    : d(new internal::FileSorterPrivate(line_end))
    {}
  FileSorter::~FileSorter()
  {
    delete d;
  }
  void FileSorter::filter_stdin()
  {
    FileScanner scanner(d->blockmap);
    for(std::string buf; std::getline(std::cin, buf); ) {
      if(scanner(buf))
	d->write(buf);
    }
  }
  void FileSorter::print_sorted()
  {
    blockmap_t& blockmap = d->blockmap;
    for(blockmap_t::iterator i = blockmap.begin(); i != blockmap.end(); ) {
      d->write(i->second.first);
      blk64_t offset = i->second.second;
      blockmap.erase(i);
      i = blockmap.lower_bound(offset);
      if(i == blockmap.end())
	i = blockmap.begin();
    }
  }
} // namespace fs
