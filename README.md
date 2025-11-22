# Kate-Ollama
[![License](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)   

Experimental plugin to integrate LLM assistance in Kate via Ollama.

My primary use for this plugin is for smart autocomplete.
This is most effective when used as boilerplate generator for frequently written code, after you have written a skeleton.

Works decently with a smallish model, I tried with qwen2.5-coder:7b.
The last comment before the cursor is typically taken into account.
The more code around to imitate, the better.

## Current Features

* Fill-In-Middle based code generation, requires models supporting fill-in-middle through ollama (e.g. qwen2.5-coder, codestral)

  * `Ctrl + ;`: insert code at the current cursor position.

  * `Ctrl + Shift + ;`: when the model is going off on a tangent, you can stop it.

* Separate chat widget, with history, for interacting with the model in the classic conversation style.
  
## Planned Features

 * Improve UI of chat panel, option of pasting code straight to editor, maybe syntax highlight for code...
 
 * Separate model selection and system prompt for completion vs chat

 * Wishlist: some features 'inspired' from qodeassist

## Installation instructions

Build and install:

```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja ../
ninja
ninja install
```
