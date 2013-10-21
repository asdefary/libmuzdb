#ifndef __MUZDB_AVFORMAT_HPP__
#define __MUZDB_AVFORMAT_HPP__

extern "C" {
#include <libavformat/avformat.h>
}

namespace muzdb {

class AVParser : public Parser {
private:
	const Path filename;
	Metadata meta;

public:
	AVParser(const Path &filename);

	void parse();
	const Metadata &metadata() const;
};

} // namespace muzdb

#endif /* __MUZDB_AVFORMAT_HPP__ */

