#include "common.hpp"

#include "parser.hpp"

namespace muzdb {

MTrack::MTrack(const Path &path, const Path &ref_path)
	: path(path), ref_path(ref_path)
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

const TimeInfo &MTrack::time() const
{
	return time_info;
}

boost::shared_ptr<MTrack> TrackGen::operator()() const
{
	return boost::make_shared<MTrack>(trk);
}

} // namespace muzdb

