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
#include "caps.hpp"
#include "verbose.hpp"
#include <sys/capability.h>
// using ::cap_*
#include <sys/prctl.h>
// using ::prctl
#include <sys/types.h>
#include <unistd.h>
// using ::setuid, ::getuid

namespace fs {
	namespace internal {
		struct CapabilitiesPrivate
		{
			::cap_t caps;
			Notifier* notifier;
			CapabilitiesPrivate(Notifier* notifier)
				: caps(::cap_get_proc()),
				  notifier(notifier)
			{}
			~CapabilitiesPrivate()
			{ cap_free(caps); }
			bool caps_unallocated() const
			{
				notifier->log("Capabilities not supported. Necessary"
							  " permissions may be missing");
				return true;
			}
			bool caps_unsupported() const
			{
				notifier->errno_error("configuring required capabilities."
									  " Necessary permissions may be missing");
				return true;
			}
		};
	}
	Capabilities::Capabilities(Notifier* notifier)
		: d(new internal::CapabilitiesPrivate(notifier))
	{}
	Capabilities::~Capabilities()
	{}
	bool Capabilities::drop_suid()
	{
		if(! d->caps)
			return d->caps_unallocated();
		if(::cap_clear(d->caps))
			return d->caps_unsupported();
		cap_value_t retain = CAP_DAC_READ_SEARCH;
		if(::cap_set_flag(d->caps, CAP_PERMITTED, 1 /*n flags*/, &retain,
						  CAP_SET))
			return d->caps_unsupported();
		if(::cap_set_proc(d->caps)
		   || ::prctl(PR_SET_KEEPCAPS, 1)
		   || ::setuid(::getuid()))
			return d->caps_unsupported();
		return false;
	}
	bool Capabilities::raise_caps()
	{
		if(! d->caps)
			return true;
		cap_value_t permission = CAP_DAC_READ_SEARCH;
		if(::cap_set_flag(d->caps, CAP_EFFECTIVE, 1 /*n flags*/, &permission,
						  CAP_SET))
			return true;
		return ::cap_set_proc(d->caps);
	}
	bool Capabilities::drop_caps()
	{
		if(! d->caps)
			return true;
		cap_value_t permission = CAP_DAC_READ_SEARCH;
		if(::cap_set_flag(d->caps, CAP_EFFECTIVE, 1 /*n flags*/, &permission,
						  CAP_CLEAR))
			return true;
		return ::cap_set_proc(d->caps);
	}
	RaisedCaps::RaisedCaps(Capabilities* c)
		: d(c)
	{
		d->raise_caps();
	}
	RaisedCaps::~RaisedCaps()
	{
		d->drop_caps();
	}
} // namespace fs
