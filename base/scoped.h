// scoped.h
// 2012/03/15
//

#pragma once


namespace base
{
	template <class T>
	class _BlockAddRefReleaseOnCComPtr : public T
	{
	private:
		STDMETHOD(QueryInterface)(REFIID iid, void** object) = 0;
		STDMETHOD_(ULONG, AddRef)()=0;
		STDMETHOD_(ULONG, Release)()=0;
	};

	template <class T, const IID* interface_id = &__uuidof(T)>
	class ScopedComPtr// : public scoped_refptr<Interface>
	{
	protected:
		T* p;

	public:
		ScopedComPtr() : p(NULL) {
		}

		ScopedComPtr(T* _ptr) : p(_ptr) {
			if(p){
				p->AddRef();
			}
		}

		~ScopedComPtr()
		{
			Release();
		}

		void Release() {
			T* t = p;
			if (t) {
				p = NULL;
				t->Release();
			}
		}

		void Attach(T* _ptr) {
			//DCHECK(!p);
			p = _ptr;
		}

		T* Detach() {
			T* t = p;
			p = NULL;
			return t;
		}

		T** Receive() {
			//LOG_IF(!m_ptr, ERROR) << "Object leak. Pointer must be NULL";
			return &p;
		}

		bool IsEqualObject(IUnknown* pOther) throw()
		{
			if (p == NULL && pOther == NULL)
				return true;	// They are both NULL objects

			if (p == NULL || pOther == NULL)
				return false;	// One is NULL the other is not

			ScopedComPtr<IUnknown> punk1;
			ScopedComPtr<IUnknown> punk2;
			p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
			pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);
			return punk1 == punk2;
		}

		template <class Q>
		HRESULT QueryInterface(Q** q) {
			//DCHECK(q != NULL);
			//DCHECK(p != NULL);
			return p->QueryInterface(q);
		}
		HRESULT QueryInterface(const IID& iid, void** obj) {
			//DCHECK(obj != NULL);
			//DCHECK(p != NULL);
			return p->QueryInterface(iid, obj);
		}


		ScopedComPtr<T>& operator=(const ScopedComPtr<T>& r) {
			return *this = r.p;
		}

		_BlockAddRefReleaseOnCComPtr<T>* operator->() const {
			//DCHECK(m_ptr != NULL);
			return reinterpret_cast<_BlockAddRefReleaseOnCComPtr<T>*>(p);
		}

		operator T*() const throw() { return p; }
		T& operator*() const { return *p; }
		T** operator&() throw() { return &p; }

		static const IID& iid() {
			return *interface_id;
		}
	};



	template<typename T>
	class ScopedCoMem
	{
	public:
		ScopedCoMem() : mem_ptr_(NULL) {}
		ScopedCoMem(T* _ptr) : mem_ptr_(_ptr) {}
		~ScopedCoMem() {
			Reset(NULL);
		}

		T** operator&() {  // NOLINT
			//DCHECK(mem_ptr_ == NULL);  // To catch memory leaks.
			return &mem_ptr_;
		}

		operator T*() {
			return mem_ptr_;
		}

		T* operator->() {
			DCHECK(mem_ptr_ != NULL);
			return mem_ptr_;
		}

		const T* operator->() const {
			//DCHECK(mem_ptr_ != NULL);
			return mem_ptr_;
		}

		void Reset(T* ptr) {
			if (mem_ptr_) {
				::CoTaskMemFree(mem_ptr_);
			}
			mem_ptr_ = ptr;
		}

	private:
		T* mem_ptr_;

		DISALLOW_COPY_AND_ASSIGN(ScopedCoMem);
	};
}

