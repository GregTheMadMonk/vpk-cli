#include "vpk-cli.hpp"
#include "manager.hpp"

#include <cstring>	// For strcmp (command-line arguments comparsion)
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

/* MAIN */
int main(int argc, char** argv) {
	if (argc < 2) {
		// Run with no arguments
		print_help();
		return 1;
	}

	string dir_file = "";
	string resource = "";
	string sel_ext = "";

	bool print_version = false;	// Print VPK version
	bool list_vpk = false;		// List VPK dir
	bool res_info = false;		// Print resource info
	string res_output = "";		// Output file name
	
	// Options parser
	const map<string, char> longopts = {
		{ "help", 'h' },
		{ "version", 'v' },
		{ "list", 'l' },
		{ "info", 'i' },
		{ "ext", 'e' },
		{ "output", 'o' },
	};
	const string opts = "hvlie:o:";

	string opt = " ";
	string opt_string = "";
	for (int i = 1; i < argc; ++i) {
		string v = argv[i];
		opt = " ";
		if (v.length() == 2) {
			if (v.at(0) == '-') {
				opt = string(1, v.at(1));
			}
		} else if (v.length() > 2) {
			if (v.at(0) == '-') {
				if (v.at(1) == '-') {
					try {
						opt = string(1, longopts.at(v.substr(2)));
					} catch (const out_of_range& e) {
						opt = " ";
					}
				} else opt = v.substr(1);
			} else opt = " ";
		} else opt = " "; // v.length() = 1

		if (opt.at(0) == ' ') opt_string = v;
		else {
			auto p = opts.find(opt);
			char next = 0;
			if (p + 1 < opts.length()) next = opts.at(p + 1);
			opt_string = "";
			if (next == ':') if (i < argc - 1) {
				++i;
				opt_string = argv[i];
			}
		}

		// Process the arguments
		for (const char& optc : opt) switch (optc) {
			case 'h':
				print_help();
				return 0;
			case 'v':
				print_version = true;
				break;
			case 'l':
				list_vpk = true;
				break;
			case 'i':
				res_info = true;
				break;
			case 'e':
				sel_ext = opt_string;
				break;
			case 'o':
				res_output = opt_string;
				break;
			case ' ':
				if (dir_file == "")		dir_file = opt_string;
				else if (resource == "")	resource = opt_string;
				break;
		}
	}

	if (dir_file == "") {
		cerr << "No dir file specified!" << endl;
		return 1;
	}

	vpk_manager vpk;
	vpk.dir_load(dir_file);

	if (print_version) cout << "VPK version: " << vpk.header.version << endl;
	if (list_vpk)
		for (const auto& e : vpk.tree) cout << e.ext << " " << e.path << e.name << endl;

	// Everything blow here corresponds to resource processing
	// Therefore, check if resource even was specified
	if ((res_info || (res_output != "")) && (resource == "")) {
		cerr << "No resource specified!" << endl;
		return 1;
	}

	int sel = -1;
	int matches = 0;
	for (int i = 0; i < vpk.tree.size(); ++i) {
		const auto& e = vpk.tree.at(i);
		if (resource == e.path + e.name) {
			if ((sel_ext != "") && (sel_ext != e.ext)) continue;
			matches++;
			sel = i;
			if (res_info) {
				cout << "On index: " << sel << endl;
				cout << "Resource: " << resource << endl;
				cout << "Extension: " << e.ext << endl;
				cout << "CRC: " << e.crc << endl;
				cout << "Preload bytes: " << e.preload_bytes << endl;
				cout << "Archive index: " << e.arch_index << endl;
				cout << "Offset: " << e.offset << endl;
				cout << "Length: " << e.length << endl;
			}
		}
	}

	if (res_output != "") {
		if (matches > 1) {
			cerr << "More than one resource named " << resource << endl;
			cerr << "Please, specify extension with -e flag" << endl;
			return 1;
		}

		ofstream out(res_output, ios_base::binary | ios_base::trunc | ios_base::out);
		for (const auto& c : vpk.get_file(vpk.tree.at(sel))) out.write(&c, 1);
		out.close();
	}

	return 0;
}

/* DEFINE FUNCTIONS */
void print_help() {
	cout << "Usage: vpk-cli <dir_file> [-hlv] [<resource_path> [-i] [-o <filename]]" << endl;
	cout << "\t-h, --help - display this message and exit" << endl;
	cout << "\t-l, --list - list resource names" << endl;
	cout << "\t-v, --version - display VPK version" << endl;
	cout << "\t-i, --info - display resource info" << endl;
	cout << "\t-o, --output - specify output filename to save resource to" << endl;
}
