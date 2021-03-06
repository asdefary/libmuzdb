#include "common.hpp"

#include "parser.hpp"
#include "avformat.hpp"
#include "cue.hpp"

#include "cdetect.hpp"

#include <string>
#include <map>

namespace muzdb {

void avformat_get_data(AVFormatContext *ctx, MTrack &trk)
{
	AVDictionary *dict = ctx->metadata;
	AVDictionaryEntry *entry(NULL);

	while (entry = av_dict_get(dict, "", entry, AV_DICT_IGNORE_SUFFIX)) {
		trk.push(boost::locale::to_lower(entry->key), CDetect(entry->value).convert(entry->value));
	}
}

AVParser::AVParser(const Path &filename)
	: filename(filename)
{
}

void AVParser::parse()
{
	meta.clear();

	AVFormatContext *ctx(NULL);

	if (avformat_open_input(&ctx, filename.string().c_str(), NULL, NULL) < 0) {
		throw std::runtime_error(filename.string());
	}

	if (avformat_find_stream_info(ctx, NULL) < 0) {
		throw std::runtime_error(filename.string());
	}

	BOOST_AUTO(trk, MTrack(filename, filename));

	avformat_get_data(ctx, trk);

	BOOST_AUTO(stream,	ctx->streams[0]);
	BOOST_AUTO(cctx,	stream->codec);
	BOOST_AUTO(time_base,	stream->time_base);

	trk.push_fin("format",	ctx->iformat->name);

	trk.push_fin("bitrate",		boost::lexical_cast<std::string>(ctx->bit_rate));
	trk.push_fin("sample_rate",	boost::lexical_cast<std::string>(cctx->sample_rate));
	trk.push_fin("channels",		boost::lexical_cast<std::string>(cctx->channels));
	trk.push_fin("bits_per_sample",	boost::lexical_cast<std::string>(cctx->bits_per_raw_sample));

	trk.set_time(stream->start_time * 1000 * time_base.num / time_base.den,
		stream->duration * 1000 * time_base.num / time_base.den, -1);

	BOOST_AUTO(tg, TrackGen(trk));

	meta.push_back(tg());

	if (trk.fields().find("cuesheet") != trk.fields().end()) {
		BOOST_AUTO(data, (*trk.fields().find("cuesheet")).second);
		BOOST_AUTO(ps, CueParser(filename, data));

		ps.parse();
		BOOST_AUTO(m, ps.metadata());

		meta.insert(meta.end(), m.begin(), m.end());
	}

	avformat_close_input(&ctx);
}

const Metadata &AVParser::metadata() const
{
	return meta;
}

} // namespace muzdb

