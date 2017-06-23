# Warframe Packages Deparser

A C++ console program designed as a utility to read `Packages.txt` datamined 
from Warframe game files.

## Getting Started

These instructions will set the project up and run the application on your
local machine. Note that this project only supports the Windows platform.

### Prerequisites

* [CMake 3.8.0+](https://cmake.org/download/)
* Any C++14-supported compiler
    * [MinGW-w64](https://sourceforge.net/projects/mingw-w64/)
* A datamined `Packages.txt`

### Compiling and Running

Run CMake in the project root.
```
<path_to_cmake>\cmake.exe -G "CodeBlocks - MinGW Makefiles" .
```

Then run Make in the project root
```
make
```

Run the program with `warframe_packages_deparser.exe --version` and ensure it 
outputs something similar to as follows:
```
Warframe Packages Deparser 0.6.0-beta.1
Copyright (C) 2017 David Mak
Licensed under MIT.
```

### Distributing the Binary

It is strongly not recommended to distribute the binary outside of this 
repository, as this utility may or may not violate the Terms of Service and/or
End-User License Agreement of Warframe. 

If you understand the risks and would still like to distribute the binary, 
continue reading.

Remove all previous CMake generated files
```
rmdir /s CMakeFiles
del CMakeCache.txt
```

Run CMake in the project root, with the following flags
```
<path_to_cmake>\cmake.exe -G "CodeBlocks - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .
```

Then run Make in the project root
```
make
```

The generated binary will contain the required libraries, and can now be distributed.

## Usage

If you have a `Packages.txt` file which has headers resembling this:
```
~FullPackageName=/EE/Types/Base/Applet
```
Execute the program like this:
```
warframe_packages_deparser.txt -f <path_to_packages_file>
```

On the other hand, if you have a `Packages.txt` file which has headers 
resembling this:
```
[/EE/Types/Base/Applet]
```
Execute the program like this:
```
warframe_packages_deparser.txt --legacy -f <path_to_packages_file>
```

## Versioning

[SemVer](http://semver.org/) is used for all releases. For all releases, see 
[tags on this repository](https://github.com/Derppening/warframe_packages_deparser/tags).

## Authors

* **David Mak** - [Derppening](https://github.com/Derppening)

## License

This project is licensed under the MIT license - see 
[LICENSE](https://github.com/Derppening/amyspeak/blob/master/LICENSE) for 
details.

GNU libstdc++ is licensed under 
[GNU GPL v3](https://gcc.gnu.org/onlinedocs/libstdc++/manual/license.html).
