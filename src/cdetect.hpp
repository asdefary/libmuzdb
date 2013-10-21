#ifndef __MUZDB_CDETECT_HPP__
#define __MUZDB_CDETECT_HPP__

namespace muzdb {

class CDetect {
private:
	std::string charset;

public:
	CDetect(const std::string &str);
	
	std::string convert(const std::string &str);
};

} // namespace muzdb

#endif /* __MUZDB_CDETECT_HPP__ */

