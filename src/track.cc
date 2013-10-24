#include "common.hpp"

#include "parser.hpp"

#include <ctime>

namespace muzdb {

static int64_t uid_seed;

void init_uid()
{
	uid_seed = time(NULL);
}

static TrackUID gen_uid()
{
	static int value = 0;

	return (TrackUID){ uid_seed, value++ };
}

MTrack::MTrack(const MTrack &trk)
	: path(trk.path)
	, ref_path(trk.ref_path)
	, flds(trk.flds)
	, muid(gen_uid())
	, time_info(trk.time_info)
{
}

MTrack::MTrack(const Path &path, const Path &ref_path)
	: path(path)
	, ref_path(ref_path)
	, muid(gen_uid())
{
}

MTrack::MTrack(const Path &path, const Path &ref_path, const TrackUID &tuid)
	: path(path)
	, ref_path(ref_path)
	, muid(tuid)
{
}

void MTrack::set_time(int start, int duration, int end)
{
	set_time((TimeInfo){ start, duration, end });
}

void MTrack::set_time(const TimeInfo &ti)
{
	time_info = ti;
}

void MTrack::push(const std::pair<std::string, std::string> &field)
{
	flds.insert(field);
}

void MTrack::push(const std::string &key, const std::string &value)
{
	flds.insert(std::make_pair(key, value));
}

void MTrack::erase(const std::string &key)
{
	flds.erase(key);
}

void MTrack::set(const std::map<std::string, std::string> &fields)
{
	flds = fields;
}

const Path &MTrack::filename() const
{
	return path;
}

const Path &MTrack::ref_filename() const
{
	return ref_path;
}

const std::map<std::string, std::string> &MTrack::fields() const
{
	return flds;
}

const TrackUID &MTrack::uid() const
{
	return muid;
}

const TimeInfo &MTrack::time() const
{
	return time_info;
}

boost::shared_ptr<MTrack> TrackGen::operator()() const
{
	return boost::make_shared<MTrack>(trk);
}

} // namespace muzdb

