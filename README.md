# Kate-Ollama
[![License](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)   

Experimental plugin to integrate LLM assistance in Kate via Ollama.

## Current Features

My primary use for this is for smart autocomplete, so this is, for now, the only feature.
This is most effective when used as boilerplate generator for frequently written code, after you have written the skeleton.

Works decently with a smallish model, I tried with qwen2.5-coder:7b.
The last comment before the cursor is typically taken into account.
The more code around to imitate, the better.

* Fill-In-Middle based code generation, requires models supporting fill-in-middle through ollama (e.g. qwen2.5-coder, codestral)

  * `Ctrl + ;`: insert code at the current cursor position.

  * `Ctrl + Shift + ;`: when the model is going off on a tangent, you can stop it.

## Planned Features

 * Tabbed panel (from the parent fork https://github.com/tfks/kate-ollama) lets the user query Ollama outside of the editor. Presently broken, will fix.

 * Improve UI of tabbed panel, giving it more chat like behavior, retaining the option of outputting straight to editor

 * Wishlist: some features 'inspired' from qodeassist

## Installation instructions

Build and install:

```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja ../
ninja
ninja install
```
