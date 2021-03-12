#include "circuit.hpp"
#include "cmd_parser.hpp"



const std::vector<std::string> cmd_table = \
{    
    "quit",     \
    "read",     \
    "pc",       \
    "lev",      \
    "plev",     \
    "logicsim",  \
    "plog",     \
	"dfs",		\
	"pfs",		\
	"rfl",      \
	"rtg",      \
	"podem",    \
	"dalg",      \
	"atpg_det",	\
	"atpg",	\
};


int find_cmd(std::vector<std::string> cmd, std::string input_str)
{
	std::vector<std::string>::iterator iter;
	int index = -1;
	transform(input_str.begin(), input_str.end(), input_str.begin(), ::tolower);
	for(iter = cmd.begin(); iter != cmd.end(); iter++)
	{
		index++;
		if(*iter == input_str)
		{
			return index;
		}
	}
	return -1;
}


