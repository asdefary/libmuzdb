#ifndef __MUZDB_TRACK_HPP__
#define __MUZDB_TRACK_HPP__

namespace muzdb {

class MTrack : public Track {
private:
	Path path;
	Path ref_path;

	std::map<std::string, std::string> flds;

	const TrackUID muid;
	TimeInfo time_info;

public:
	MTrack(const MTrack &trk);
	MTrack(const Path &path, const Path &ref_path);
	MTrack(const Path &path, const Path &ref_path, const TrackUID &tuid);

	void set_time(int start, int duration, int end);
	void set_time(const TimeInfo &ti);

	void push(const std::pair<std::string, std::string> &field);
	void push(const std::string &key, const std::string &value);

	void erase(const std::string &key);

	void set(const std::map<std::string, std::string> &fields);

	const Path &filename() const;
	const Path &ref_filename() const;

	const std::map<std::string, std::string> &fields() const;
	
	const TrackUID &uid() const;
	const TimeInfo &time() const;
};

class TrackGen {
private:
	const MTrack trk;

public:
	TrackGen(const MTrack &trk) : trk(trk) {}
	TrackGen(const Path &filename, const Path &ref_filename)
		: trk(MTrack(filename, ref_filename)) {}

	boost::shared_ptr<MTrack> operator()() const;
};

void init_uid();

} // namespace muzdb

#endif /* __MUZDB_TRACK_HPP__ */

