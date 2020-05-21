#include <string>
#include <iostream>
#include "clock_point_save/clock_point_save.h"

using namespace std;

int main(int argc, char** argv) {
	string image_dir;
	string save_file_path;
	string image_name_format;

	if (argc < 3) {
		cout << "argvs: image_dir, save_file_path, image_name_format " << endl;

		return -1;
	}

	image_dir = argv[1];
	save_file_path = argv[2];
	image_name_format = argv[3];

	clock_point_save save;

	if (save.save_image_dir(image_dir, image_name_format, save_file_path))
	{
		return 0;
	}

	return -2;
}