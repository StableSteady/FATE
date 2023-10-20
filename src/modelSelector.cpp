#include <fstream>
#include <iostream>
#include <utility>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "modelSelector.h"

enum class ModelType {
	Ascension,
	Costume
};

static void to_json(json& j, const Character& character) {
	j = json{ {"name", character.name},
				{"ascension", character.ascension},
				{"costume", character.costume}
	};
}

static void from_json(const json& j, Character& character) {
	j.at("name").get_to(character.name);
	for (const auto& arr : j["ascension"]) {
		character.ascension[arr[0].get<int>()] = arr[1].get<std::string>();
	}
	for (const auto& arr : j["costume"]) {
		character.costume[arr[0].get<int>()] = arr[1].get<std::string>();
	}
}

ModelSelector::ModelSelector() : modelChanged(false), characterIndex(NO_SELECTION), modelType(ModelType::Ascension), ascensionIndex(NO_SELECTION), costumeIndex(NO_SELECTION) {
	initCharacterList();
}

void ModelSelector::draw() {
	ImGui::Begin("Model Select");
	characterSelectionDropdown();
	updateButton();
	modelSelection();
	renderButton();
	ImGui::End();
}

void ModelSelector::initCharacterList() {
	fs::path characterListPath = fs::current_path().append(characterListFilename);
	if (!fs::is_regular_file(characterListPath)) {
		createAndLoadCharacterList();
	} else {
		loadCharacterListFromDisk();
	}
}

void ModelSelector::createAndLoadCharacterList() {
	downloader.updateNiceData();
	
	std::ifstream niceDataStream(downloader.niceDataPath);
	makeCharacterListFromNiceData(json::parse(niceDataStream));
	writeCharacterListToDisk();
}

void ModelSelector::makeCharacterListFromNiceData(const json& niceDataJSON) {
	characters.clear();
	for (const auto& servant : niceDataJSON) {
		Character character;
		character.name = servant["name"];
		for (const auto& ascension : servant["extraAssets"]["spriteModel"]["ascension"].items()) {
			fs::path url(ascension.value().get<std::string>());
			int id = std::stoi(url.parent_path().stem().string());
			if (!character.ascension.contains(id)) {
				character.ascension[id] = "Ascension";
			}
			character.ascension[id] += ' ' + ascension.key();
		}
		if (servant["extraAssets"]["spriteModel"].contains("costume")) {
			for (const auto& costume : servant["extraAssets"]["spriteModel"]["costume"].items()) {
				int id = std::stoi(costume.key());
				character.costume[id] = servant["profile"]["costume"][costume.key()]["shortName"];
			}
		}

		characters.push_back(std::move(character));
	}
}

void ModelSelector::loadCharacterListFromDisk() {
	std::ifstream characterListStream(fs::current_path().append(characterListFilename));
	json characterListJSON = json::parse(characterListStream);
	characters = characterListJSON.get<std::vector<Character>>();
}

void ModelSelector::writeCharacterListToDisk() {
	std::cout << "Writing list to disk...\n";
	const auto startTime = std::chrono::system_clock::now();
	std::ofstream characterListStream(characterListFilename);
	json characterListJSON(characters);
	characterListStream << characterListJSON;
	const auto endTime = std::chrono::system_clock::now();
	std::cout << "Finished writing in " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << " seconds.\n";
}

void ModelSelector::characterSelectionDropdown() {
	if (ImGui::BeginCombo("##characterSelect", characterIndex != NO_SELECTION ? characters[characterIndex].name.c_str() : "Select Character")) {
		for (int i = 0; i < characters.size(); i++) {
			const bool isSelected = (characterIndex == i);
			if (ImGui::Selectable(characters[i].name.c_str(), isSelected)) {
				characterIndex = i;
				modelType = ModelType::Ascension;
				ascensionIndex = NO_SELECTION;
				costumeIndex = NO_SELECTION;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void ModelSelector::updateButton() {
	ImGui::SameLine();
	if (ImGui::Button("Update")) {
		createAndLoadCharacterList();
	}
}

void ModelSelector::renderButton() {
	if (characterIndex == NO_SELECTION or (ascensionIndex == NO_SELECTION and costumeIndex == NO_SELECTION)) {
		return;
	}
	if (ImGui::Button("Load")) {
		modelChanged = true;
		modelPath = getSelectedModelPath();
	}
}

void ModelSelector::modelSelection() {
	if (characterIndex == NO_SELECTION) {
		return;
	}

	ImGui::RadioButton("Ascension", (int*)&modelType, static_cast<int>(ModelType::Ascension));
	if (modelType == ModelType::Ascension) {
		costumeIndex = NO_SELECTION;

		ImGui::Indent(10.f);
		for (const auto& [id, ascensions] : characters[characterIndex].ascension) {
			ImGui::RadioButton(ascensions.c_str(), &ascensionIndex, id);
		}
		ImGui::Indent(-10.f);

		if (characters[characterIndex].costume.empty()) {
			ImGui::BeginDisabled();
			ImGui::RadioButton("Costume", (int*)&modelType, static_cast<int>(ModelType::Costume));
			ImGui::EndDisabled();
		} else {
			ImGui::RadioButton("Costume", (int*)&modelType, static_cast<int>(ModelType::Costume));
		}

		ImGui::BeginDisabled();
		ImGui::Indent(10.f);
		for (const auto& [id, name] : characters[characterIndex].costume) {
			ImGui::RadioButton(name.c_str(), &costumeIndex, id);
		}
		ImGui::Indent(-10.f);
		ImGui::EndDisabled();
	} else {
		ascensionIndex = NO_SELECTION;

		ImGui::BeginDisabled();
		ImGui::Indent(10.f);
		for (const auto& [id, ascensions] : characters[characterIndex].ascension) {
			ImGui::RadioButton(ascensions.c_str(), &ascensionIndex, id);
		}
		ImGui::Indent(-10.f);
		ImGui::EndDisabled();

		ImGui::RadioButton("Costume", (int*)&modelType, static_cast<int>(ModelType::Costume));
		ImGui::Indent(10.f);
		for (const auto& [id, name] : characters[characterIndex].costume) {
			ImGui::RadioButton(name.c_str(), &costumeIndex, id);
		}
		ImGui::Indent(-10.f);
	}
}

fs::path ModelSelector::getSelectedModelPath() {
	fs::path characterDataFolder = fs::current_path().append("Models").append(characters[characterIndex].name);
	const std::string modelName = modelType == ModelType::Ascension ? characters[characterIndex].ascension[ascensionIndex] : characters[characterIndex].costume[costumeIndex];
	characterDataFolder.append(modelName);

	const std::string index = std::to_string(modelType == ModelType::Ascension ? ascensionIndex : costumeIndex);
	
	return downloader.getModel(index, characterDataFolder);
}
