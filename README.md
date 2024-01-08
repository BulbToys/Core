# BulbToys/Base
This repository contains all the required files to create a BulbToys trainer, including [MinHook](https://github.com/TsudaKageyu/minhook) and [ImGui](https://github.com/ocornut/imgui).

### Disclaimer
This repository is currently an **early work-in-progress**, any and all information is subject to change without notice and may be missing or incorrect.

# Usage
### Requirements
As BulbToys is an internal cheat, it currently only supports DirectX 9 games. This project does not include an injector, so you will need to use your own (eg. [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) or [Xenos](https://github.com/DarthTon/Xenos)).

### Setup
*todo*

### Namespaces
BulbToys exposes 6 namespaces for the user:
- Global (`::`) - Contains helper & utility functions and classes/structs, such as `Read<T>` and `Write<T>`
- `Hooks::` - Contains function hooks, applied either as soon as the trainer is initialized (eg. DirectX, for rendering) or on demand through other means
  - It is also meant to contain user-defined game function hooks
- `GUI::` - Contains the backbone of the GUI, such as ImGui initialization and rendering
- `ImGui::` - Contains ImGui tools used to create the GUI
  - BulbToys-specific widgets are prefixed with `ImGui::BulbToys_`
  - If creating game-specific ImGui widgets, it is recommended to prefix them with `ImGui::BulbToys_Game`
- `Patches::` - Contains byte array patches, applied either as soon as the trainer is initialized or on demand through other means
When making your own BulbToys trainer, it is recommended to create your own namespace for game function calls and classes/structs, as well as any helper functions and classes/structs that utilise said game information (eg. [BulbToys/NFSC](https://github.com/BulbToys/NFSC) uses the `NFSC::` namespace).