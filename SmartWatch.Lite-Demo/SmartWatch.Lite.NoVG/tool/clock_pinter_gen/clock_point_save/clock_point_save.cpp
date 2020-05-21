#include "clock_point_save.h"
#include "../file_name_sort.hpp"
#include "image_loader/image_loader_stb.h"

#ifdef WIN32
#include <Windows.h>
#include <io.h>
#else
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std;
#define FILE_FORMAT_PNG "png"
#define SET_OFFSET(size, count, offset) offset += (size * count)

#define WRITE_DATA_TO_FILE(data, size, count, file, offset) {\
fwrite(data, size, count, file);\
SET_OFFSET(size, count, offset);\
}\

clock_point_save::clock_point_save()
{
}

clock_point_save::~clock_point_save()
{
}

#ifdef WIN32
void clock_point_save::get_dir_file_list(const string& dir, list<string>& file_list) {
	string new_file;
	string path = dir;
	path.append("\\*.");
	path.append(FILE_FORMAT_PNG);
	WIN32_FIND_DATAA info;
	memset(&info, 0, sizeof(WIN32_FIND_DATAA));
	HANDLE hfile = FindFirstFileA(path.c_str(), &info);
	if (hfile == NULL || hfile == INVALID_HANDLE_VALUE) return;

	do {
		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(info.cFileName, ".") != 0 && strcmp(info.cFileName, "..") != 0) {
				string new_dir = dir;
				new_dir.append("\\");
				new_dir.append(info.cFileName);

				get_dir_file_list(new_dir, file_list);
			}
		}
		else {
			new_file = dir;
			new_file.append("\\");
			new_file.append(info.cFileName);
			if (_access(new_file.c_str(), 0) != -1) {
				file_list.push_back(new_file);
			}
		}
	} while (FindNextFileA(hfile, &info));
}
#else
void clock_point_save::get_dir_file_list(const string& dir, list<string>& file_list) {
	DIR* dp;
	string format;
	string childpath;
	struct stat statbuf;
	struct dirent* entry;
	if ((dp = opendir(dir.c_str())) != NULL)
	{
		while ((entry = readdir(dp)) != NULL)
		{
			if (stat(entry->d_name, &statbuf) >= 0 && S_ISDIR(statbuf.st_mode))
			{
				if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
				{
					continue;
				}

				childpath = dir;
				childpath.append("/");
				childpath.append(entry->d_name);
				get_dir_file_list(childpath, file_list);
			}
			else
			{
				childpath = dir;
				childpath.append("/");
				childpath.append(entry->d_name);

				format = childpath.substr(childpath.find_last_of('.') + 1);

				if (format == FILE_FORMAT_PNG)
				{
					file_list.push_back(childpath);
				}
			}
		}

		closedir(dp);
	}
}
#endif  // WINDOW

static inline unsigned char* get_image_file_data(const std::string& image_file, unsigned int& data_length)
{
	long size = 0;
	FILE* in_File;
	unsigned char* buff = NULL;

#ifdef WIN32
	fopen_s(&in_File, image_file.c_str(), "rb");
#else
	in_File = fopen(image_file.c_str(), "rb");
#endif // WINDOW

	if (in_File == NULL) return NULL;

	fseek(in_File, 0, SEEK_END);
	size = ftell(in_File);
	fseek(in_File, 0, SEEK_SET);

	buff = new unsigned char[size];
	fread(buff, size, 1, in_File);
	fclose(in_File);

	data_length = size;

	return buff;
}

static inline bool load_image(const std::string& image_file, bitmap_t* image)
{
	ret_t ret = RET_FAIL;
	unsigned int buff_length = 0;

	bool require_bgra = false;
	bool enable_bgr565 = false;

	unsigned char* buff = get_image_file_data(image_file, buff_length);

	if (buff != NULL)
	{
		ret = stb_load_image(0, buff, buff_length, image, FALSE, FALSE, FALSE);
		delete buff;
	}

	return ret == RET_OK;
}

uint32_t clock_point_save::save_image_info_list(FILE* save_file, const std::list<clock_point_info_t>& clock_point_info_list)
{
	uint32_t len = 0;
	list<clock_point_info_t>::const_iterator _Last = clock_point_info_list.end();
	list<clock_point_info_t>::const_iterator _First = clock_point_info_list.begin();
	for (; _First != _Last; ++_First)
	{
		WRITE_DATA_TO_FILE(&_First->base, 1, sizeof(clock_point_info_base_t), save_file, len);
		WRITE_DATA_TO_FILE(_First->data, _First->base.len, sizeof(uint32_t), save_file, len);

		delete _First->data;
	}

	return len;
}

bool clock_point_save::save_image_data_list(const std::list<std::string>& file_list, const std::string& save_file_path)
{
	bitmap_t image;
	uint8_t size = 0;
	uint32_t offset = 0;
	FILE* save_file = NULL;
	clock_point_head_t* clock_point_head_list = NULL;
	list<string>::const_iterator _Last = file_list.end();
	list<string>::const_iterator _First = file_list.begin();

#ifdef WIN32
	fopen_s(&save_file, save_file_path.c_str(), "wb+");
#else
	save_file = fopen(save_file_path.c_str(), "wb+");
#endif // WINDOW

	return_value_if_fail(save_file != NULL && file_list.size() > 0 && file_list.size() < 0xff, false);

	size = (uint8_t)file_list.size();
	clock_point_head_list = new clock_point_head_t[size];
	memset(clock_point_head_list, 0, size * sizeof(clock_point_head_t));

	WRITE_DATA_TO_FILE(&size, 1, sizeof(int8_t), save_file, offset);
	WRITE_DATA_TO_FILE(clock_point_head_list, size, sizeof(clock_point_head_t), save_file, offset);

	for (int8_t ii = 0; _First != _Last; ++_First, ii++)
	{
		std::list<clock_point_info_t> clock_point_info_list;

		clock_point_head_list[ii].point = offset;
		clock_point_info_list.clear();
		if (load_image(*_First, &image))
		{
			uint32_t p = 0;
			bool is_save = false;
			uint32_t image_len = image.h * image.w;
			uint32_t* src_data = (uint32_t*)bitmap_lock_buffer_for_read(&image);

			for (uint32_t i = 0; i < image_len; i++, src_data++)
			{
				uint8_t* s = (uint8_t*)src_data;
				if (!is_save && s[3] > 0x8)
				{
					p = i;
					is_save = true;
				}
				else if (is_save && s[3] <= 0x8)
				{
					clock_point_info_t info;
					info.base.p = p;
					info.base.len = i - p;
					info.data = new uint32_t[info.base.len];
					memcpy(info.data, src_data - info.base.len, info.base.len * sizeof(uint32_t));

					clock_point_info_list.push_back(info);
					is_save = false;
				}
			}
			bitmap_unlock_buffer(&image);

			uint32_t image_data_len = save_image_info_list(save_file, clock_point_info_list);
			clock_point_head_list[ii].len = image_data_len;
			clock_point_head_list[ii].width = image.w;
			clock_point_head_list[ii].height = image.h;
			SET_OFFSET(1, image_data_len, offset);
		}
	}

	fseek(save_file, sizeof(int8_t), SEEK_SET);
	WRITE_DATA_TO_FILE(clock_point_head_list, size, sizeof(clock_point_head_t), save_file, offset);

	if (clock_point_head_list != NULL)
	{
		free(clock_point_head_list);
		clock_point_head_list = NULL;
	}

	fclose(save_file);

	return true;
}

bool clock_point_save::save_image_dir(const std::string& files_dir, const std::string image_name_format, const std::string& save_file_path)
{
	std::list<std::string> file_list;
	file_list.clear();
	get_dir_file_list(files_dir, file_list);

	file_list.sort(FileNameSort(image_name_format));

	return save_image_data_list(file_list, save_file_path);
}

