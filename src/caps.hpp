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
#ifndef _FS_CAPS_HPP
#define _FS_CAPS_HPP
#include "memberptr.hpp"

namespace fs {
	class Notifier;
	namespace internal {
		class CapabilitiesPrivate;
	}
	class Capabilities
	{
		OwningMemberPtr<internal::CapabilitiesPrivate> d;
		Capabilities(const Capabilities&);
		Capabilities& operator=(const Capabilities&);
	public:
		Capabilities(Notifier* notifier);
		~Capabilities();
		bool drop_suid();
		bool raise_caps();
		bool drop_caps();
	};
	class RaisedCaps
	{
		MemberPtr<Capabilities> d;
		RaisedCaps(const RaisedCaps&);
		RaisedCaps& operator=(const RaisedCaps&);
	public:
		RaisedCaps(Capabilities*);
		~RaisedCaps();
	};
} // namespace fs

#endif /* _FS_CAPS_HPP */
