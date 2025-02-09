#include "lib/spotify/audiofeature.hpp"

/*
 * The values here are based off the average distribution, not necessarily
 * what actually "makes sense". The data used to be available from the
 * Web API, but has been removed in the new design.
 */

lib::spt::audio_feature::audio_feature(::audio_feature feature, float value)
	: feature(feature),
	value(value)
{
	switch (feature)
	{
		case ::audio_feature::acousticness:
			name = acousticness();
			break;

		case ::audio_feature::danceability:
			name = danceability();
			break;

		case ::audio_feature::energy:
			name = energy();
			break;

		case ::audio_feature::instrumentalness:
			name = instrumentalness();
			break;

		case ::audio_feature::key:
			break;

		case ::audio_feature::liveness:
			name = liveness();
			break;

		case ::audio_feature::loudness:
			name = loudness(minimum, maximum);
			break;

		case ::audio_feature::mode:
			break;

		case ::audio_feature::speechiness:
			name = speechiness();
			break;

		case ::audio_feature::tempo:
			name = tempo(maximum);
			break;

		case ::audio_feature::valence:
			name = valence();
			break;

		case ::audio_feature::time_signature:
			name = time_signature(maximum);
			break;

		default:
			lib::log::warn("Invalid value: {}", get_feature());
			break;
	}
}

lib::spt::audio_feature::audio_feature(audio_key key)
	: feature(::audio_feature::key),
	name(to_string(key)),
	value(static_cast<float>(key))
{
	// These values doesn't really make sense here,
	// but it looks better
	minimum = static_cast<float>(audio_key::c);
	maximum = static_cast<float>(audio_key::b);
}

lib::spt::audio_feature::audio_feature(audio_mode mode)
	: feature(::audio_feature::mode),
	name(to_string(mode)),
	value(static_cast<float>(mode))
{
	// These values doesn't really make sense here,
	// but it looks better
	minimum = static_cast<float>(audio_mode::minor);
	maximum = static_cast<float>(audio_mode::major);
}

auto lib::spt::audio_feature::acousticness() const -> std::string
{
	constexpr float no = 0.1F;
	constexpr float probably_no = 0.4F;
	constexpr float probably_yes = 0.7F;

	return value <= no
		? "Not acoustic"
		: value <= probably_no
			? "Probably not acoustic"
			: value <= probably_yes
				? "Probably acoustic"
				: "Acoustic";
}

auto lib::spt::audio_feature::danceability() const -> std::string
{
	constexpr float no_lower = 0.4F;
	constexpr float no_higher = 0.85F;

	return value <= no_lower || value >= no_higher
		? "Not suitable"
		: "Suitable";
}

auto lib::spt::audio_feature::energy() const -> std::string
{
	constexpr float very_low = 0.4F;
	constexpr float low = 0.65F;
	constexpr float medium = 0.8F;
	constexpr float high = 0.95F;

	return value <= very_low
		? "Very low"
		: value <= low
			? "Low"
			: value <= medium
				? "Medium"
				: value <= high
					? "High"
					: "Very high";
}

auto lib::spt::audio_feature::instrumentalness() const -> std::string
{
	constexpr float no = 0.05F;

	return value <= no
		? "Not instrumental"
		: "Instrumental";
}

auto lib::spt::audio_feature::liveness() const -> std::string
{
	constexpr float no = 0.05F;
	constexpr float probably_no = 0.15F;
	constexpr float probably_yes = 0.4F;

	return value <= no
		? "Not live"
		: value <= probably_no
			? "Probably not live"
			: value <= probably_yes
				? "Probably live"
				: "Live";
}

auto lib::spt::audio_feature::loudness(float &min, float &max) const -> std::string
{
	constexpr float min_value = -60.F;
	constexpr float max_value = 0.F;

	min = min_value;
	max = max_value;

	constexpr float very_quiet = -25.F;
	constexpr float quiet = -15.F;
	constexpr float loud = -2.5F;

	return value <= very_quiet
		? "Very quiet"
		: value <= quiet
			? "Quiet"
			: value <= loud
				? "Loud"
				: "Very loud";
}

auto lib::spt::audio_feature::speechiness() const -> std::string
{
	constexpr float speech = 0.6F;
	constexpr float music = 0.8F;

	return value <= speech
		? "Speech"
		: value >= music
			? "Music"
			: "Mixed";
}

auto lib::spt::audio_feature::tempo(float &max) const -> std::string
{
	constexpr float max_value = 250.F;
	max = max_value;

	constexpr float slow = 60.F;
	constexpr float fast = 150.F;

	return value <= slow
		? "Slow"
		: value >= fast
			? "Fast"
			: "Average";
}

auto lib::spt::audio_feature::valence() const -> std::string
{
	constexpr float sad = 0.2F;
	constexpr float happy = 0.7F;

	return value <= sad
		? "Sad"
		: value >= happy
			? "Happy"
			: "Mixed";
}

auto lib::spt::audio_feature::time_signature(float &max) const -> std::string
{
	// Time signature doesn't really have a maximum value?
	max = value;

	return lib::fmt::format("{} m", static_cast<int>(value));
}

auto lib::spt::audio_feature::get_feature() const -> std::string
{
	switch (feature)
	{
		case ::audio_feature::unknown:
			return "Unknown";

		case ::audio_feature::acousticness:
			return "Acousticness";

		case ::audio_feature::danceability:
			return "Danceability";

		case ::audio_feature::energy:
			return "Energy";

		case ::audio_feature::instrumentalness:
			return "Instrumentalness";

		case ::audio_feature::key:
			return "Key";

		case ::audio_feature::liveness:
			return "Liveness";

		case ::audio_feature::loudness:
			return "Loudness";

		case ::audio_feature::mode:
			return "Mode";

		case ::audio_feature::speechiness:
			return "Speechiness";

		case ::audio_feature::tempo:
			return "Tempo";

		case ::audio_feature::time_signature:
			return "Time signature";

		case ::audio_feature::valence:
			return "Valence";

		default:
			return "Unknown";
	}
}

auto lib::spt::audio_feature::get_name() const -> std::string
{
	return name;
}

auto lib::spt::audio_feature::get_value() const -> float
{
	return value;
}

auto lib::spt::audio_feature::get_min() const -> float
{
	return minimum;
}

auto lib::spt::audio_feature::get_max() const -> float
{
	return maximum;
}

auto lib::spt::audio_feature::get_description() const -> std::string
{
	if (get_min() == 0.F && get_max() == 1.F)
	{
		return lib::fmt::format("{}%", get_value() * 100.F);
	}

	std::string suffix;
	if (feature == ::audio_feature::loudness)
	{
		suffix = " dB";
	}
	else if (feature == ::audio_feature::tempo)
	{
		suffix = " BPM";
	}
	else if (feature == ::audio_feature::time_signature)
	{
		suffix = " m";
	}

	return lib::fmt::format("{}{}", get_value(), suffix);
}

auto lib::spt::audio_feature::to_string(audio_key key) -> std::string
{
	switch (key)
	{
		case audio_key::c:
			return "C";

		case audio_key::c_sharp:
			return "C♯, D♭";

		case audio_key::d:
			return "D";

		case audio_key::d_sharp:
			return "D♯, E♭";

		case audio_key::e:
			return "E";

		case audio_key::f:
			return "F";

		case audio_key::f_sharp:
			return "F♯, G♭";

		case audio_key::g:
			return "G";

		case audio_key::g_sharp:
			return "G♯, A♭";

		case audio_key::a:
			return "A";

		case audio_key::a_sharp:
			return "A♯, B♭";

		case audio_key::b:
			return "B";

		default:
			return "?";
	}
}

auto lib::spt::audio_feature::to_string(audio_mode mode) -> std::string
{
	switch (mode)
	{
		case audio_mode::minor:
			return "Minor";

		case audio_mode::major:
			return "Major";

		default:
			return "Unknown";
	}
}
