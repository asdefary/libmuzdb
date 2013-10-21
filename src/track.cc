#include "common.hpp"

#include "parser.hpp"

namespace muzdb {

MTrack::MTrack(const Path &path, const Path &ref_path)
	: path(path), ref_path(ref_path)
{
}

void MTrack::set_time(int start, int duration, int end)
{
	time_info = (TimeInfo){ start, duration, end };
}

void MTrack::push(const std::pair<std::string, std::string> &field)
{
	flds.insert(field);
}

void MTrack::push(const std::string &key, const std::string &value)
{
	flds.insert(std::make_pair(key, value));
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

const TimeInfo &MTrack::time() const
{
	return time_info;
}

boost::shared_ptr<MTrack> TrackGen::operator()() const
{
	return boost::make_shared<MTrack>(trk);
}

} // namespace muzdb

