//	file_path.cc
//
//

#include "stdafx.h"
#include "file_path.h"


namespace base
{
	typedef FilePath::StringType StringType;
	const FilePath::CharType kStringTerminator = FILE_PATH_LITERAL('\0');

	FilePath::FilePath() {
	}

	FilePath::FilePath(const FilePath& that)
		: path_(that.path_)
	{
	}

	FilePath::FilePath(const StringType& path)
		: path_(path)
	{
		StringType::size_type nul_pos = path_.find(kStringTerminator);
		if (nul_pos != StringType::npos)
			path_.erase(nul_pos, StringType::npos);
	}

	FilePath::~FilePath() {
	}

	FilePath& FilePath::operator=(const FilePath& that) {
		path_ = that.path_;
		return *this;
	}
} // namespace base.

