#include "common.hpp"

#include "cdetect.hpp"

extern "C" {
#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>
}

namespace muzdb {

CDetect::CDetect(const std::string &str)
{
	UErrorCode err = U_ZERO_ERROR;

	BOOST_AUTO(d, ucsdet_open(&err));

	if (U_FAILURE(err)) {
		throw std::runtime_error(u_errorName(err));
	}

	ucsdet_setText(d, str.c_str(), str.length(), &err);

	BOOST_AUTO(m, ucsdet_detect(d, &err));

	charset = ucsdet_getName(m, &err);

	ucsdet_close(d);
}

std::string CDetect::convert(const std::string &str)
{
	UErrorCode err = U_ZERO_ERROR;

	size_t target_len(str.length() * 10);
	int32_t conv_len(0);
	int32_t sig_len(0);

	boost::shared_ptr<char> target(new char[target_len]);

	conv_len = ucnv_convert("UTF-8", charset.c_str(), target.get(), target_len, str.c_str(), str.length(), &err); 

	if (U_FAILURE(err)) {
		throw std::runtime_error(u_errorName(err));
	}

	ucnv_detectUnicodeSignature(target.get(), conv_len, &sig_len, &err);

	if (U_FAILURE(err)) {
		throw std::runtime_error(u_errorName(err));
	}

	return std::string(target.get(), sig_len, conv_len);
}

} // namespace muzdb

