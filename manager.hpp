#pragma once
#ifndef _VPK_MANAGER
#define _VPK_MANAGER

#include <fstream>
#include <string>
#include <vector>

constexpr auto size_uint	= sizeof(unsigned int);
constexpr auto size_ushort	= sizeof(unsigned short);

struct vpk_entry {
	std::string ext;
	std::string path;
	std::string name;

	unsigned int	preload_where;	// Where preload data starts

	// All of this data could've been read directly into memory from file
	// Sadly, I don't know how to remove padding from structs 😥
	unsigned int	crc;
	unsigned short	preload_bytes;
	unsigned short	arch_index;
	unsigned int	offset;
	unsigned int	length;
	unsigned short	terminator;

	static constexpr unsigned short	E_TERMINATOR	= 0xffff;
	static constexpr unsigned short	E_ARCH_CURRENT	= 0x7fff;
};

struct vpk_header {
	unsigned int signature;
	unsigned int version;

	unsigned int tree_size;
	struct {
		unsigned int file_data;
		unsigned int archive_md5;
		unsigned int other_md5;
		unsigned int signature;
	} section_sizes;

	static constexpr unsigned int H_SIGNATURE = 0x55aa1234;	// Expected signature
};

class vpk_manager {
	std::string	filename;	// Dir VPK filename
	std::ifstream	dir;		// Dir VPK read stream

	std::string read_string();	// Read string from tree section
	void read_file(vpk_entry& entry);
	unsigned int header_size();
	public:
	vpk_header header;
	std::vector<vpk_entry> tree;	// Dir tree
	
	void dir_load(const std::string& dir_file);	// Load dir VPK
	void close();

	std::vector<char> get_file(const vpk_entry& entry); // Get file bytes
};

#endif
