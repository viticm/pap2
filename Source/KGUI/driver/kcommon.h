#ifndef UI_DRIVER_K_COMMON_H
#define UI_DRIVER_K_COMMON_H

#include "../common/tstring.h"

namespace UI
{
	class _tstring_iless
	{
	public:
		bool operator() (_tstring const& src1, _tstring const& src2) const
		{
			const size_t len1 = src1.size();
			const size_t len2 = src2.size();
			if (len1 < len2)
				return true;
			if (len1 > len2)
				return false;
			return _tcsicmp(src1.c_str(), src2.c_str()) < 0;
		}
	};

}

#endif // UI_DRIVER_K_COMMON_H

