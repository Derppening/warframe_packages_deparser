# Warframe Packages Deparser

A C++ console program designed as a utility to read `Packages.txt` datamined 
from Warframe game files.

## Getting Started

These instructions will set the project up and run the application on your
local machine.

### Prerequisites

* [CMake 3.8.0+](https://cmake.org/download/)
* Any C++14-supported compiler
    * [GCC](https://gcc.gnu.org/)
    * [Clang](https://clang.llvm.org/)
* [GNU Make](https://www.gnu.org/software/make/)
* A datamined `Packages.txt`

### Compiling and Running

#### Windows

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
Warframe Packages Deparser 0.9.0
Copyright (C) 2017 David Mak
Licensed under MIT.
```

#### *nix

Run CMake in the project root.
```
cmake -G "CodeBlocks - Unix Makefiles" .
```

Then run Make in the project root
```
make
```

Run the program with `./warframe_packages_deparser --version` and ensure it 
outputs something similar to as follows:
```
Warframe Packages Deparser 0.9.0
Copyright (C) 2017 David Mak
Licensed under MIT.
```

### Distributing the Binary

It is strongly not recommended to distribute the binary outside of this 
repository, as this utility may (or may not) violate the Terms of Service and/or
End-User License Agreement of Warframe. 

If you understand the risks and would still like to distribute the binary, 
continue reading.

Remove all previous CMake generated files
```
make clean
```

Run CMake in the project root, with the following flags
```
cmake -G "CodeBlocks - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .
```

Then run Make in the project root
```
make
```

The generated binary will contain the required libraries, and can now be distributed.

## Usage

If you have a `Packages.txt` file with headers like this
```
~FullPackageName=/EE/Types/Base/Applet
```
Or if you have a diff-optimized `Packages.txt` file generated by this application
```
FullPackageName=/EE/Types/Base/Applet
```
Execute the program like this
```
warframe_packages_deparser.txt -f <path_to_packages_file>
```

Other formats of `Packages.txt` are not supported.

Help for command line arguments can be found using the `--help` flag.

## Versioning

[SemVer](http://semver.org/) is used for all releases. For all releases, see 
[tags on this repository](https://github.com/Derppening/warframe_packages_deparser/releases).

## Authors

* **David Mak** - [Derppening](https://github.com/Derppening)

## License

This project is licensed under the MIT license - see 
[LICENSE](https://github.com/Derppening/amyspeak/blob/master/LICENSE) for 
details.

GNU libstdc++ is licensed under 
[GNU GPL v3](https://gcc.gnu.org/onlinedocs/libstdc++/manual/license.html).
