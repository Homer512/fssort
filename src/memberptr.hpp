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
#ifndef _FS_MEMBERPTR_HPP
#define _FS_MEMBERPTR_HPP

#include <utility>
// using std::swap
namespace fs {
	namespace internal {
		template<class T>
		class MemberPtrBase
		{
		public:
			typedef T element_type;
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
		private:
			T* m;
		protected:
			MemberPtrBase(const MemberPtrBase& o)
				: m(o.m)
			{}
			MemberPtrBase& operator=(const MemberPtrBase& o)
			{
				m = o.m;
				return *this;
			}
			MemberPtrBase(T* member = pointer())
				: m(member)
			{}
			pointer& ptr_ref()
			{ return m; }
			void swap(MemberPtrBase& o)
			{
				using std::swap;
				swap(m, o.m);
			}
		public:
			T& operator*()
			{ return *m; }
			const T& operator*() const
			{ return *m; }
			T* operator->()
			{ return m; }
			const T* operator->() const
			{ return m; }
			T* get()
			{ return m; }
			const T* get() const
			{ return m; }
			T* release()
			{
				T* tmp = m;
				m = pointer();
				return tmp;
			}
			operator bool() const
			{ return m; }
		};
	} // namespace internal
	/**
	 * A non-owning smart pointer that has the const behavior of a reference,
	 * i.e. if the pointer is const, then element references are also const
	 *
	 * \tparam T the contained type
	 */
	template<class T>
	class MemberPtr: public internal::MemberPtrBase<T>
	{
	public:
		MemberPtr(T* member = internal::MemberPtrBase<T>::pointer())
			: internal::MemberPtrBase<T>(member)
		{}
		void reset(T* member = internal::MemberPtrBase<T>::pointer())
		{ this->ptr_ref() = member; }
		void swap(MemberPtr& o)
		{ this->internal::MemberPtrBase<T>::swap(o); }
	};
	template<class T>
	void swap(MemberPtr<T>& a, MemberPtr<T>& b)
	{ a.swap(b); }

	/**
	 * An owning smart pointer that has the const behavior of a reference,
	 * i.e. if the pointer is const, then element references are also const
	 *
	 * The contained object is deleted when the smart pointer is destroyed
	 *
	 * \tparam T the contained type
	 */
	template<class T>
	class OwningMemberPtr: public internal::MemberPtrBase<T>
	{
		OwningMemberPtr(const OwningMemberPtr&);
		OwningMemberPtr& operator=(const OwningMemberPtr&);
	public:
		OwningMemberPtr(T* member = internal::MemberPtrBase<T>::pointer())
			: internal::MemberPtrBase<T>(member)
		{}
		~OwningMemberPtr()
		{
			delete this->ptr_ref();
		}
		void reset(T* member = internal::MemberPtrBase<T>::pointer())
		{ 
			delete this->ptr_ref();
			this->ptr_ref() = member;
		}
		void swap(OwningMemberPtr& o)
		{ this->internal::MemberPtrBase<T>::swap(o); }
	};
	template<class T>
	void swap(OwningMemberPtr<T>& a, OwningMemberPtr<T>& b)
	{ a.swap(b); }

} // namespace fs

#endif /* _FS_MEMBERPTR_HPP */
