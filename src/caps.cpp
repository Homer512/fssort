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

namespace {
	class CapState
	{
	protected:
		CapState() {}
		~CapState() {}
		CapState(const CapState&) {}
		CapState& operator=(const CapState&) { return *this; }
		bool fail(fs::internal::CapabilitiesPrivate&);
		bool apply_caps(fs::internal::CapabilitiesPrivate&);
	public:
		virtual bool
		is_supported(const fs::internal::CapabilitiesPrivate&) const = 0;
		virtual bool drop_suid(fs::internal::CapabilitiesPrivate&) = 0;
		virtual bool raise(fs::internal::CapabilitiesPrivate&) = 0;
		virtual bool drop(fs::internal::CapabilitiesPrivate&) = 0;
	};
	class InitialCapState: public CapState
	{
		bool get_flag(fs::internal::CapabilitiesPrivate&,
					  ::cap_flag_value_t&);
	public:
		virtual bool
		is_supported(const fs::internal::CapabilitiesPrivate&) const;
		virtual bool drop_suid(fs::internal::CapabilitiesPrivate&);
		virtual bool raise(fs::internal::CapabilitiesPrivate&);
		virtual bool drop(fs::internal::CapabilitiesPrivate&);
	};
	struct EffectiveCapState: CapState
	{
		virtual bool
		is_supported(const fs::internal::CapabilitiesPrivate&) const;
		virtual bool drop_suid(fs::internal::CapabilitiesPrivate&);
		virtual bool raise(fs::internal::CapabilitiesPrivate&);
		virtual bool drop(fs::internal::CapabilitiesPrivate&);
	};
	struct FailedCapState: CapState
	{
		virtual bool
		is_supported(const fs::internal::CapabilitiesPrivate&) const;
		virtual bool drop_suid(fs::internal::CapabilitiesPrivate&);
		virtual bool raise(fs::internal::CapabilitiesPrivate&);
		virtual bool drop(fs::internal::CapabilitiesPrivate&);
	};
}

namespace fs {
	namespace internal {
		struct CapabilitiesPrivate
		{
			::cap_t caps;
			CapState* state;
			EffectiveCapState effective_state;
			FailedCapState failed_state;
			InitialCapState initial_state;
			CapabilitiesPrivate()
				: caps(::cap_get_proc()),
				  state(&initial_state)
			{}
			~CapabilitiesPrivate()
			{ cap_free(caps); }
		};
	} // namespace internal
} // namespace fs
namespace {
	bool CapState::fail(fs::internal::CapabilitiesPrivate& q)
	{
		q.state = &q.failed_state;
		return true;
	}
	bool CapState::apply_caps(fs::internal::CapabilitiesPrivate& q)
	{
		if(::cap_set_proc(q.caps))
			return fail(q);
		return true;
	}
	bool InitialCapState::
	is_supported(const fs::internal::CapabilitiesPrivate& q) const
	{ return q.caps; }
	bool InitialCapState::drop_suid(fs::internal::CapabilitiesPrivate& q)
	{
		if(! q.caps || ::cap_clear(q.caps))
			return fail(q);
		cap_value_t retain = CAP_DAC_READ_SEARCH;
		if(::cap_set_flag(q.caps, CAP_PERMITTED, 1 /*n flags*/, &retain,
						  CAP_SET))
			return fail(q);
		if(::cap_set_proc(q.caps)
		   || ::prctl(PR_SET_KEEPCAPS, 1)
		   || ::setuid(::getuid()))
			return fail(q);
		q.state = &q.effective_state;
		return false;
	}
	bool InitialCapState::get_flag(fs::internal::CapabilitiesPrivate& q,
								   ::cap_flag_value_t& v)
	{
		if(! q.caps)
			return fail(q);
		if(::cap_get_flag(q.caps, CAP_DAC_READ_SEARCH, CAP_EFFECTIVE, &v))
			return fail(q);
		return false;
	}
	bool InitialCapState::raise(fs::internal::CapabilitiesPrivate& q)
	{
		::cap_flag_value_t v;
		return get_flag(q, v) || v == CAP_SET;
	}
	bool InitialCapState::drop(fs::internal::CapabilitiesPrivate& q)
	{
		::cap_flag_value_t v;
		return get_flag(q, v) || v == CAP_CLEAR;
	}
	bool EffectiveCapState::
	is_supported(const fs::internal::CapabilitiesPrivate&) const
	{ return true; }
	bool EffectiveCapState::drop_suid(fs::internal::CapabilitiesPrivate&)
	{ return false; }
	bool EffectiveCapState::raise(fs::internal::CapabilitiesPrivate& q)
	{
		cap_value_t permission = CAP_DAC_READ_SEARCH;
		if(::cap_set_flag(q.caps, CAP_EFFECTIVE, 1 /*n flags*/, &permission,
						  CAP_SET))
			return fail(q);
		return apply_caps(q);
	}
	bool EffectiveCapState::drop(fs::internal::CapabilitiesPrivate& q)
	{
		cap_value_t permission = CAP_DAC_READ_SEARCH;
		if(::cap_set_flag(q.caps, CAP_EFFECTIVE, 1 /*n flags*/, &permission,
						  CAP_CLEAR))
			return fail(q);
		return apply_caps(q);
	}
	bool FailedCapState::
	is_supported(const fs::internal::CapabilitiesPrivate&) const
	{ return false; }
	bool FailedCapState::drop_suid(fs::internal::CapabilitiesPrivate&)
	{ return true; }
	bool FailedCapState::raise(fs::internal::CapabilitiesPrivate&)
	{ return true; }
	bool FailedCapState::drop(fs::internal::CapabilitiesPrivate&)
	{ return true; }
} // namespace
namespace fs {
	Capabilities::Capabilities()
		: d(new internal::CapabilitiesPrivate())
	{}
	Capabilities::~Capabilities()
	{}
	bool Capabilities::drop_suid()
	{ return d->state->drop_suid(*d); }
	bool Capabilities::raise_caps()
	{ return d->state->raise(*d); }
	bool Capabilities::drop_caps()
	{ return d->state->drop(*d); }
	bool Capabilities::is_supported() const
	{ return d->state->is_supported(*d); }
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
