#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <cpr/cpr.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "downloadManager.h"

DownloadManager::DownloadManager() {
	initNiceDataPath();
	initNiceDataURL();
}

fs::path DownloadManager::getModel(const std::string& id, const fs::path& modelFolder) const {
	if (!fs::is_directory(modelFolder)) {
		fs::create_directories(modelFolder);
	}

	fs::path modelPath = modelFolder;
	modelPath.append(id);
	if (!fs::is_regular_file(modelPath)) {
		downloadModel(id, modelPath);
		extractModel(id, modelPath);
	}

	fs::path extractedModelPath = modelFolder;
	extractedModelPath.append("chr.fbx");
	return extractedModelPath;
}

void DownloadManager::updateNiceData() const {
	if (!fs::is_regular_file(niceDataPath) or niceDataUpdateAvailable()) {
		downloadNiceData();
	}
}

void DownloadManager::initNiceDataPath() {
	niceDataPath = fs::current_path().append(niceDataFilename);
}

void DownloadManager::initNiceDataURL() {
	niceDataURL.append(exportURL).append("/").append(niceDataFilename);
}

void DownloadManager::downloadNiceData() const {
	std::cout << "Downloading nice data from Atlas Academy...\n";
	std::chrono::time_point startTime = std::chrono::system_clock::now();
	const cpr::Response response = cpr::Get(cpr::Url{ niceDataURL }, cpr::ReserveSize{ 1024 * 1024 * 64 });
	std::chrono::time_point endTime = std::chrono::system_clock::now();
	if (!response.error and response.status_code == 200) {
		std::cout << "Downloaded data in " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << " seconds.\n";
		std::ofstream niceData(niceDataPath);
		niceData << response.text;
	}
	else {
		std::cout << "Data download failed\n";
	}
}

void DownloadManager::downloadModel(const std::string& id, const fs::path& path) const {
	std::string downloadURL = assetURL;
	downloadURL.append("/").append(id).append("/").append(id);

	std::cout << "Downloading servant data from Atlas Academy...\n";
	const std::chrono::time_point startTime = std::chrono::system_clock::now();
	std::ofstream characterData(path, std::ios::binary);
	auto session = cpr::Session();
	session.SetUrl(downloadURL);
	cpr::Response response = session.Download(characterData);
	const std::chrono::time_point endTime = std::chrono::system_clock::now();
	std::cout << "Downloaded data in " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << " seconds.\n";
}

void DownloadManager::extractModel(const std::string& id, const fs::path& path) const {
	std::cout << "Extracting character model...\n";
	const std::chrono::time_point startTime = std::chrono::system_clock::now();
	fs::path assetStudioPath = fs::current_path().append("AssetStudioCLI").append("AssetStudioCLI.exe");
	std::string command = assetStudioPath.string() + " " + "\"" + path.string() + "\"";
	std::system(command.c_str());
	const std::chrono::time_point endTime = std::chrono::system_clock::now();
	std::cout << "Extracted model in " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << " seconds.\n";
}

bool DownloadManager::niceDataUpdateAvailable() const {
	int64_t niceDataTimestamp;
	const cpr::Response response = cpr::Get(cpr::Url{ infoURL }, cpr::Timeout(5000));
	if (!response.error and response.status_code == 200) {
		json info = json::parse(response.text);
		niceDataTimestamp = info["JP"]["timestamp"];
	}
	else {
		std::cout << "Update check failed with status code " + response.status_code << '\n';
		niceDataTimestamp = INT64_MIN; // set to min to avoid updating later
	}
	using namespace std::chrono;
	int64_t localTimestamp = duration_cast<seconds>(clock_cast<utc_clock>(fs::last_write_time(niceDataPath)).time_since_epoch()).count();
	return localTimestamp < niceDataTimestamp;
}
