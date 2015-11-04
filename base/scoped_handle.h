//	scoped_handle.h
//
//

#ifndef __BASE_WIN_SCOPED_HANDLE_H__
#define __BASE_WIN_SCOPED_HANDLE_H__

#include "base_types.h"
#include "log/logging.h"

//
// CAVEATS
//
// If you include a move-only type as a field inside a class that does not
// explicitly declare a copy constructor, the containing class's implicit
// copy constructor will change from Containing(const Containing&) to
// Containing(Containing&).  This can cause some unexpected errors.
//
//   http://llvm.org/bugs/show_bug.cgi?id=11528
//
// The workaround is to explicitly declare your copy constructor.
//
#define MOVE_ONLY_TYPE_FOR_CPP_03(type, rvalue_type) \
private: \
struct rvalue_type { \
	explicit rvalue_type(type* object) : object(object) {} \
	type* object; \
}; \
	type(type&); \
	void operator=(type&); \
public: \
	operator rvalue_type() { return rvalue_type(this); } \
	type Pass() { return type(rvalue_type(this)); } \
private:


#ifndef DCHECK
#	define DCHECK(x)
#endif


namespace base {

	// TODO(rvargas): remove this with the rest of the verifier.
#if defined(COMPILER_MSVC)
	// MSDN says to #include <intrin.h>, but that breaks the VS2005 build.
	extern "C" {
		void* _ReturnAddress();
	}
#define BASE_WIN_GET_CALLER _ReturnAddress()
#elif defined(COMPILER_GCC)
#define BASE_WIN_GET_CALLER __builtin_extract_return_addr(\\
	__builtin_return_address(0))
#endif

	// Generic wrapper for raw handles that takes care of closing handles
	// automatically. The class interface follows the style of
	// the ScopedStdioHandle class with a few additions:
	//   - IsValid() method can tolerate multiple invalid handle values such as NULL
	//     and INVALID_HANDLE_VALUE (-1) for Win32 handles.
	//   - Receive() method allows to receive a handle value from a function that
	//     takes a raw handle pointer only.
	template <class Traits, class Verifier>
	class GenericScopedHandle {
		MOVE_ONLY_TYPE_FOR_CPP_03(GenericScopedHandle, RValue)

	public:
		typedef typename Traits::Handle Handle;

		// Helper object to contain the effect of Receive() to the function that needs
		// a pointer, and allow proper tracking of the handle.
		class Receiver {
		public:
			explicit Receiver(GenericScopedHandle* owner)
				: handle_(Traits::NullHandle()),
				owner_(owner) {}
			~Receiver() { owner_->Set(handle_); }

			operator Handle*() { return &handle_; }

		private:
			Handle handle_;
			GenericScopedHandle* owner_;
		};

		GenericScopedHandle() : handle_(Traits::NullHandle()) {}

		explicit GenericScopedHandle(Handle handle) : handle_(Traits::NullHandle()) {
			Set(handle);
		}

		// Move constructor for C++03 move emulation of this type.
		GenericScopedHandle(RValue other) : handle_(Traits::NullHandle()) {
			Set(other.object->Take());
		}

		~GenericScopedHandle() {
			Close();
		}

		bool IsValid() const {
			return Traits::IsHandleValid(handle_);
		}

		// Move operator= for C++03 move emulation of this type.
		GenericScopedHandle& operator=(RValue other) {
			if (this != other.object) {
				Set(other.object->Take());
			}
			return *this;
		}

		void Set(Handle handle) {
			if (handle_ != handle) {
				Close();

				if (Traits::IsHandleValid(handle)) {
					handle_ = handle;
				}
			}
		}

		Handle Get() const {
			return handle_;
		}

		operator Handle() const {
			return handle_;
		}

		// This method is intended to be used with functions that require a pointer to
		// a destination handle, like so:
		//    void CreateRequiredHandle(Handle* out_handle);
		//    ScopedHandle a;
		//    CreateRequiredHandle(a.Receive());
		Receiver Receive() {
			DBG_IF(ERROR, !Traits::IsHandleValid(handle_)) << "Handle must be NULL";
			return Receiver(this);
		}

		// Transfers ownership away from this object.
		Handle Take() {
			Handle temp = handle_;
			handle_ = Traits::NullHandle();
			if (Traits::IsHandleValid(temp)) {
			}
			return temp;
		}

		// Explicitly closes the owned handle.
		void Close() {
			if (Traits::IsHandleValid(handle_)) {

				if (!Traits::CloseHandle(handle_)){
					//CHECK(false);
				}

				handle_ = Traits::NullHandle();
			}
		}

	private:
		Handle handle_;
	};

#undef BASE_WIN_GET_CALLER

	// The traits class for Win32 handles that can be closed via CloseHandle() API.
	class HandleTraits {
	public:
		typedef HANDLE Handle;

		// Closes the handle.
		static bool CloseHandle(HANDLE handle) {
			return ::CloseHandle(handle) != FALSE;
		}

		// Returns true if the handle value is valid.
		static bool IsHandleValid(HANDLE handle) {
			return handle != NULL && handle != INVALID_HANDLE_VALUE;
		}

		// Returns NULL handle value.
		static HANDLE NullHandle() {
			return NULL;
		}

	private:
		DISALLOW_IMPLICIT_CONSTRUCTORS(HandleTraits);
	};

	// Performs actual run-time tracking.
	class VerifierTraits {
	public:
		typedef HANDLE Handle;

	private:
		DISALLOW_IMPLICIT_CONSTRUCTORS(VerifierTraits);
	};

	typedef GenericScopedHandle<HandleTraits, VerifierTraits> ScopedHandle;

}  // namespace base

#endif // __BASE_WIN_SCOPED_HANDLE_H__

