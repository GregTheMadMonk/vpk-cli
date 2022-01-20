# vpk-cli
A simple command-line utility to read and extract Valve VPK files.
Doesn't really do anything else.

## Building
**vpk-cli** uses a Makefile. To build, run `make`.

## Usage

```
Usage: vpk-cli <dir_file> [-hlv] [<resource_path> [-i] [-o <filename]]
	-h, --help - display this message and exit
	-l, --list - list resource names
	-v, --version - display VPK version
	-i, --info - display resource info
	-o, --output - specify output filename to save resource to
```

## TODO
[Valve Developer Community](https://developer.valvesoftware.com/wiki/VPK_File_Format#Notes) article describes skipping in the file format that is not considered by **vpk-cli**.

Also, **vpk-cli** doesn't currently provide any means to verify files and archives via checksums and signatures present in VPKv2.
