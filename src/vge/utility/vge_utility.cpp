#include <vge_utility.h>
#include <fstream>
#include <sstream>

std::string
VGE::ReadFile(const char* filepath)
{
    std::ifstream file(filepath);
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}
