#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "json.hpp"
#include "utils/error_exit.hpp"
#include "utils/usage.hpp"

using json = nlohmann::json;
using string = std::string;

struct config {
	json _raw;

	string work_dir;
	string store_dir;

	void read(const std::string &config_path){
		std::ifstream config_file(config_path);
		config_file >> _raw;

		work_dir = _raw["work_dir"];
		store_dir = _raw["store_dir"];
	}

	void read(const int argc, const char **argv){
		std::string config_file;

		if (argc < 2) {
			config_file = "/etc/deduper/config.json";
		} else {
			if(argc > 2){
				print_usage();
			}
			config_file = argv[1];
			try{
				read(config_file);
			} catch(std::exception &e){
				errExit("Exception: %s", e.what());
			}
		}

	}
};

