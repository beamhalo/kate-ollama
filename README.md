# Kate-Ollama
[![License](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)   

Experimental plugin to integrate LLM assistance in Kate via Ollama.

## Current Features

* Prompt based code generation (from the original https://github.com/Mte90/kate-ollama)

  * `Ctrl + /`: prints `// AI: `
  * `Ctrl + ;`: execute Ollama with the `generate` endpoint, so doesn't have memory of what was already executed
  * `Ctrl + Shift + ;`: execute Ollama with the `generate` endpoint, but with the whole content injected before the prompt

* Tabbed panel (from the parent fork https://github.com/tfks/kate-ollama) let's the user query Ollama outside of the editor. Still in development but basic functionality is there.

## Planned Features

 * Replace prompt based code generation with Fill In Middle autocomplete (requires compatible models)

 * Improve UI of tabbed panel, giving it more chat like behavior, retaining the option of outputting to edit

## Installation instructions

Build and install:

```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja ../
ninja
ninja install
```

If you are developing a symlink as root can simplify the loading of the latest plugin build:

```
ln -s /your-folder/build/plugins/kf6/ktexteditor/kateollama.so /usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/kateollama.so
```
