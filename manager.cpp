#include "manager.hpp"

#include <stdexcept>

using namespace std;

string vpk_manager::read_string() {
	string ret = "";
	char c;
	while (true) {
		dir.read(&c, 1);
		if (c == '\0') return ret;
		ret += c;
	}
	throw runtime_error("read_string didn't reach null terminator");
}

void vpk_manager::read_file(vpk_entry& entry) {
	dir.read((char*)&entry.crc, size_uint);
	dir.read((char*)&entry.preload_bytes, size_ushort);
	dir.read((char*)&entry.arch_index, size_ushort);
	dir.read((char*)&entry.offset, size_uint);
	dir.read((char*)&entry.length, size_uint);
	dir.read((char*)&entry.terminator, size_ushort);

	if (entry.terminator != vpk_entry::E_TERMINATOR)
		throw runtime_error("Unexpected entry terminator: " + to_string(entry.terminator) + " instead of " + to_string(vpk_entry::E_TERMINATOR));

	// Skip preload data for now, but remember position
	entry.preload_where = dir.tellg();
	dir.ignore(entry.preload_bytes);
}

vector<char> vpk_manager::get_file(const vpk_entry& e) {
	vector<char> ret;

	// Check if preload data exists and load it
	if (e.preload_bytes != 0) {
		dir.seekg(e.preload_where);
		char c;
		for (unsigned short i = 0; i < e.preload_bytes; ++i) {
			dir.read(&c, 1);
			ret.push_back(c);
		}
	}

	// Load the archive
	auto* arch = &dir;	// By default, seek in the same archive
	ifstream arch_other;
	unsigned int offset;
	if (e.arch_index != vpk_entry::E_ARCH_CURRENT) {
		// Open the new archive
		// Get the new archive name by replacing "_dir" in VPK file name
		auto dir_where = filename.rfind("_dir");
		string index = to_string(e.arch_index);
		while (index.length() < 3) index = "0" + index;
		string arch_path = filename.replace(dir_where, 4, "_" + index);
		arch_other.open(arch_path, ios_base::binary);

		if (!arch_other.is_open()) throw runtime_error("Cannot find archive file " + arch_path);

		arch = &arch_other;
		offset = e.offset;
	} else offset = e.offset + header.tree_size + header_size();
	
	arch->seekg(offset);
	char c;
	for (unsigned int i = 0; i < e.length; ++i) {
		arch->read(&c, 1);
		ret.push_back(c);
	}

	if (arch_other.is_open()) arch_other.close();

	return ret;
}

unsigned int vpk_manager::header_size() {
	if (header.version == 1) return size_uint * 3;
	return size_uint * 7;
}

void vpk_manager::dir_load(const string& dir_file) {
	if (dir.is_open())
		throw runtime_error("Manager asked to load VPK with other open"); // VPK is already open
	dir.open(dir_file, ios_base::binary);
	if (!dir.is_open())
		throw runtime_error("Could not open file " + dir_file);
	filename = dir_file;

	// Read and check the signature
	dir.read((char*)&header.signature, size_uint);
	if (header.signature != vpk_header::H_SIGNATURE)
		throw runtime_error("Signature differs from excepted!");

	// Read version and tree size
	dir.read((char*)&header.version, size_uint);
	dir.read((char*)&header.tree_size, size_uint);

	if (header.version == 2) {
		// VPK version 2 has additional info in the header
		dir.read((char*)&header.section_sizes.file_data, size_uint);
		dir.read((char*)&header.section_sizes.archive_md5, size_uint);
		dir.read((char*)&header.section_sizes.other_md5, size_uint);
		dir.read((char*)&header.section_sizes.signature, size_uint);
	}

	// Load the dir tree
	for (string ext = read_string(); ext != ""; ext = read_string())
		for (string path = read_string(); path != ""; path = read_string())
			for (string fname = read_string(); fname != ""; fname = read_string()) {
				vpk_entry file;
				file.ext = ext;
				file.path = path;
				if (file.path.back() != '/') file.path += '/';
				file.name = fname;

				read_file(file);

				tree.push_back(file);
			}
}

void vpk_manager::close() {
	dir.close();
	filename = "";
	tree.clear();
}
