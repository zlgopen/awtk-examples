#ifndef CLOCK_POINT_SAVE_H
#define CLOCK_POINT_SAVE_H

//#ifdef __cplusplus
// extern "C" {
//#endif
//

#include <list>
#include <string>
#include <algorithm>
#include "../../../src/clock_point_data/clock_point_types.h"

class clock_point_save {
public:
  clock_point_save();
  ~clock_point_save();

  bool save_image_data_list(const std::list<std::string> &file_list,
                            const std::string &save_file);

  bool save_image_dir(const std::string &files_dir,
                      const std::string image_name_format,
                      const std::string &save_file_path);

private:
  void clock_point_save::get_dir_file_list(const std::string &dir,
                                           std::list<std::string> &file_list);

  unsigned int save_image_info_list(
      FILE *save_file,
      const std::list<clock_point_info_t> &clock_point_info_list);
};

//#ifdef __cplusplus
//}
//#endif

#endif