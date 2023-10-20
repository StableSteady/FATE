#pragma once

#include <filesystem>
#include <map>
#include <string>

namespace fs = std::filesystem;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "downloadManager.h"

enum class ModelType;

struct Character {
	std::string name;
	std::map<int, std::string> ascension;
	std::map<int, std::string> costume;
};

class ModelSelector {
public:
	ModelSelector();
	void draw();
	bool modelChanged;
	fs::path modelPath;

private:
	void initCharacterList();
	void createAndLoadCharacterList();
	void makeCharacterListFromNiceData(const json& niceDataJSON);
	void loadCharacterListFromDisk();
	void writeCharacterListToDisk();

	void characterSelectionDropdown();
	void updateButton();
	void renderButton();
	void modelSelection();

	fs::path getSelectedModelPath();

	static constexpr const char* characterListFilename = "characterList.json";

	DownloadManager downloader;
	std::vector<Character> characters;
	int characterIndex;
	ModelType modelType;
	int ascensionIndex, costumeIndex;

	static constexpr int NO_SELECTION = -1;
};