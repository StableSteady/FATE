#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class DownloadManager {
public:
	DownloadManager();
	[[nodiscard]] fs::path getModel(const std::string& id, const fs::path& modelFolder) const;
	void updateNiceData() const;

	fs::path niceDataPath;

private:
	void initNiceDataPath();
	void initNiceDataURL();
	void downloadNiceData() const;
	void downloadModel(const std::string& id, const fs::path& path) const;
	void extractModel(const std::string& id, const fs::path& path) const;
	[[nodiscard]] bool niceDataUpdateAvailable() const;

	static constexpr const char* niceDataFilename = "nice_servant_lore_lang_en.json";
	static constexpr const char* exportURL = "https://api.atlasacademy.io/export/JP";
	static constexpr const char* assetURL = "https://static.atlasacademy.io/JP/Servants";
	static constexpr const char* infoURL = "https://api.atlasacademy.io/info";

	std::string niceDataURL;
};