# TrenchBroomBFG for DOOM-3-BFG

***This is still very much Work in Progress and not supported by the official TrenchBroom yet.***

<img src="https://i.imgur.com/3sUxOZi.jpg">

<img src="https://i.imgur.com/3g3Qmf4.jpg">


***The goal of TrenchBroomBFG is to make mapping for Doom 3 (BFG) as easy as it is for Quake.***

Doom 3 also requires some extensions in order to work with TrenchBroom. The Quake 1/2/3 communities already adopted the Valve 220 .map format in the BSP compilers and I did the same with dmap in RBDOOM-3-BFG.
I usually develop these things for RBDOOM-3-BFG but I created a separated branch for vanilla Doom 3 so everybody can adopt TrenchBroom and the new Doom 3 (Valve) .map support. You can find all relevant code snippets by looking for `TrenchBroom` in the comments

https://github.com/RobertBeckebans/DOOM-3/tree/506-TrenchBroom-interop

TrenchBroomBFG's abilities for Doom 3 are still limited at the moment and it works best with RBDOOM-3-BFG.

You can only save maps to the Doom 3 (Valve) format but you can copy paste from the vanilla Doom 3 .map format into the Doom 3 (Valve)configuration and reset your texture alignment as you want.

RBDOOM-3-BFG >= 1.4 also has the console command `convertMaptoValve220 <mapname>` which lets you easily convert any original Doom 3 (and BFG) .map and saves it with the _valve220.map suffix. This command prepares Doom 3 maps for editing in TrenchBroom and saves a lot of additional work in this TrenchBroomBFG fork.
In addition the RBDOOM-3-BFG engine framework and ingame light editor can also save light settings, new entities or placed ragdolls back to the new Doom 3 (Valve) .map format so both programs interop with each other.

Here is an overview of the changes made to TrenchBroom:

## New
* Doom 3 .map parser with brushDef3, patchDef2, patchDef3 primitives
* Doom 3 Valve .map configuration
* Quake 3 .shader parser adopted to support .mtr materials. It includes support for Doom 3 diffuse stages and the lookup for them is like in idMaterial::GetEditorImage()
* New Doom 3 OBJ parser. My TB Interop branch automatically creates OBJ files to work with TB and it also allows seamless interop with Blender 2.8x - 3.x without the need of additional model formats
* RBDOOM-3-BFG >= 1.4 ships with TB specific helper entities like misc_model or func_rotating_model to reflect TrenchBroom's clean architecture and differentiation between Brush Entities and Point Entities
* Game FGDs for Doom 3 and Doom 3 BFG
* Custom PNG icons for special entities like lights, speakers, particle emitters, info_location, target_* and so on
* After loading a map TrenchBroomBFG generates unique entity names and also fixes missing or bad "model" keys for brush based entitites
* Creating new entities like a light will automatically receive unique names like light_2
* Changed Rotation Tool behaviour to use "angles" by default
* Translucent materials that might cause BSP leaks have a purple rectangle border in the Texture Browser
* Changed OBJ exporter so exported models can be easier reimported by RBDOOM-3-BFG

## Issues
* It doesn't allow to create bezier patches at the moment so you won't be able to edit existing Doom 3 maps
* TrenchBroomBFG doesn't support brush primitives like in D3Radiant or DarkRadiant. Use the Valve 220 .map format instead
* It has no support for BFG .resource files and .bimage files. You need to call `exportImagesToTrenchBroom` in RBDoom's console once to have all images available in base/_tb/textures/ as PNG format. 
* Many entities work differently in Doom 3 if they have an origin. Brush work in D3 is usually stored in entity space and not world space. This is a major issue and not solved within TrenchBroomBFG. However as a workaround you can use the convertMapToValve220 engine command to prepare a Doom 3 map for the TrenchBroom Doom 3 (Valve) configuration.
* Doom 3's primary model formats are LWO and ASE. Assimp has problems to convert them into the Quake/Doom coordinates. (not an issue with the BFG edition)
* Some ASE models can't be loaded and materials are usually all wrong if loaded (not an issue with the BFG edition)
* TrenchBroomBFG doesn't support the "rotation" keyword and many models have the wrong orientation (not an issue with the BFG edition)
* TrenchBroomBFG breaks compatibility for other id Tech engines, e.g. entity links work between "target[num]" and "name" and not "targetname"
* Linked groups do not work yet with id Tech 4 due to name conflicts


## Releases
- Binary builds are bundled with RBDOOM-3-BFG available from Mod DB [releases](https://www.moddb.com/mods/rbdoom-3-bfg).
You can find this customized version under tools/trenchbroom/.

For feedback, questions and support join the id Tech 4 Discord: https://discord.gg/Q3E9rUFnnP , #rbdoom3bfg

You can also find weekly or monthly snapshots of this TrenchBroom version in the pinned messages of #rbdoom3bfg

<!-- 
# TrenchBroom

[![TrenchBroom Icon](app/resources/graphics/images/AppIcon.png)](https://www.youtube.com/watch?v=shcAvnYp9ow)

TrenchBroom is a modern cross-platform level editor for Quake-engine based games.

- Trailer:   https://www.youtube.com/watch?v=shcAvnYp9ow
- Website:   https://github.com/TrenchBroom/TrenchBroom
- Discord:   https://discord.gg/WGf9uve
- Twitter:   https://twitter.com/tb_leveleditor
- Video Tutorial Series:  https://www.youtube.com/playlist?list=PLgDKRPte5Y0AZ_K_PZbWbgBAEt5xf74aE
- Manual:    https://trenchbroom.github.io/manual/latest

## Features
* **General**
	- Full support for editing in 3D and in up to three 2D views
	- High performance renderer with support for huge maps
	- Unlimited Undo and Redo
	- Macro-like command repetition
	- Issue browser with automatic quick fixes
	- Point file support
	- Automatic backups
	- .obj file export
	- Free and cross platform
* **Brush Editing**
	- Robust vertex editing with edge and face splitting and manipulating multiple vertices together
	- Clipping tool with two and three points
	- Scaling and shearing tools
	- CSG operations: merge, subtract, intersect
	- UV view for easy texture manipulations
	- Precise texture lock for all brush editing operations
	- Multiple texture collections
* **Entity Editing**
	- Entity browser with drag and drop support
	- Support for FGD and DEF files for entity definitions
	- Mod support
	- Entity link visualization
	- Displays 3D models in the editor
	- Smart entity property editors
* **Supported Games**
	- Quake (Standard and Valve 220 file formats)
	- Quake 2
	- Quake 3 (partial, no patches or brush primitives yet)
	- Hexen 2
	- Daikatana
	- Generic (for custom engines)
	- More games can be supported with custom game configurations


## Releases
- Binary builds are available from [releases](https://github.com/kduske/TrenchBroom/releases).

## Compiling
- Read [Build.md](Build.md) for instructions

# Contributing
- Bug reports and feature suggestions are welcome. Please submit them at https://github.com/TrenchBroom/TrenchBroom/issues
- If you wish to contribute code or improve the documentation, please get in touch with me at kristian.duske@gmail.com.
- All help is appreciated!

# Credits
- [Qt](https://www.qt.io/)
- [FreeType](https://www.freetype.org/)
- [FreeImage](https://freeimage.sourceforge.io/)
- [GLEW](https://github.com/nigels-com/glew)
- [TinyXML](http://www.grinninglizard.com/tinyxml/)
- miniz
- [Assimp](https://www.assimp.org/)
- [Catch2](https://github.com/catchorg/Catch2)
- [CMake](https://cmake.org/)
- [vcpkg](https://www.vcpkg.io/)
- [Pandoc](https://www.pandoc.org/)
- Quake icons by [Th3 ProphetMan](https://www.deviantart.com/th3-prophetman)
- Hexen 2 icon by [thedoctor45](https://www.deviantart.com/thedoctor45)
- [Source Sans Pro](https://fonts.google.com/specimen/Source+Sans+Pro) font

## Changes
See [releases](https://github.com/TrenchBroom/TrenchBroom/releases) for latest changes.

-->