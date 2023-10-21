# FATE
Servant model viewer for Fate/Grand Order

![Arjuna Alter](https://github.com/StableSteady/FATE/blob/master/assets/Arjuna%20Alter.gif)

![Mary Anning](https://github.com/StableSteady/FATE/blob/master/assets/Mary%20Anning.gif)

![Oberon Vortigern](https://github.com/StableSteady/FATE/blob/master/assets/Oberon.gif)

## Build Instructions
```
git clone --recursive https://github.com/StableSteady/FATE.git
cd FATE
mkdir build
cd build
cmake ..
```

## Usage Instructions
A special fork of AssetStudio is required to extract the models and can be found [here](https://github.com/StableSteady/AssetStudio). Place the folder in the FATE executable directory (or working directory if debugging).

## FAQ
### The application stops responding whenever I click a button
There are some long running tasks (downloading/extracting) which can block the UI. Refer to the CLI window for specific details.

### Models are downloaded but not extracted
Make sure AssetStudioCLI directory is placed in the executable directory.

### I encountered a bug / X Servant doesn't work / The application crashed
The model viewer is under active development and not all servants have been tested. Report any bugs please.

## Credits
[Atlas Academy DB](https://apps.atlasacademy.io/db/) for the servant assets.

[AssetStudio](https://github.com/Perfare/AssetStudio) for extracting assets from the Unity3D files to a usable format.
