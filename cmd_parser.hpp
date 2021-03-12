#ifndef __CMD_PARSER_HPP__
#define __CMD_PARSER_HPP__

#include <algorithm>
#include <string>
#include <vector>


extern const std::vector<std::string> cmd_table;

int find_cmd(std::vector<std::string> cmd, std::string input_str);


#endif
