//	file_path.h
//
//

#ifndef __BASE_FILE_PATH_H__
#define __BASE_FILE_PATH_H__

#include <stddef.h>
#include <string>
#include <vector>

#include "../base_types.h"

namespace base
{
	class FilePath
	{
	public:
#if defined(OS_POSIX)
		// On most platforms, native pathnames are char arrays, and the encoding
		// may or may not be specified.  On Mac OS X, native pathnames are encoded
		// in UTF-8.
		typedef std::string StringType;
#elif defined(OS_WIN)
		// On Windows, for Unicode-aware applications, native pathnames are wchar_t
		// arrays encoded in UTF-16.
		typedef std::wstring StringType;
#else
#error os must defined first.
#endif  // OS_WIN

		typedef StringType::value_type CharType;

		FilePath();
		FilePath(const FilePath& that);
		explicit FilePath(const StringType& path);
		~FilePath();
		FilePath& operator=(const FilePath& that);

		//bool operator==(const FilePath& that) const;

		//bool operator!=(const FilePath& that) const;

		//// Required for some STL containers and operations
		//bool operator<(const FilePath& that) const {
		//	return path_ < that.path_;
		//}

		const StringType& value() const { return path_; }

		bool empty() const { return path_.empty(); }

		void clear() { path_.clear(); }

		// Returns true if |character| is in kSeparators.
		//static bool IsSeparator(CharType character);


	private:
		// Remove trailing separators from this object.  If the path is absolute, it
		// will never be stripped any more than to refer to the absolute root
		// directory, so "////" will become "/", not "".  A leading pair of
		// separators is never stripped, to support alternate roots.  This is used to
		// support UNC paths on Windows.
		void StripTrailingSeparatorsInternal();

		StringType path_;
	};
} // namespace base.


// Macros for string literal initialization of FilePath::CharType[], and for
// using a FilePath::CharType[] in a printf-style format string.
#if defined(OS_POSIX)
#define FILE_PATH_LITERAL(x) x
#define PRFilePath "s"
#define PRFilePathLiteral "%s"
#elif defined(OS_WIN)
#define FILE_PATH_LITERAL(x) L ## x
#define PRFilePath "ls"
#define PRFilePathLiteral L"%ls"
#endif  // OS_WIN


#endif // __BASE_FILE_PATH_H__

