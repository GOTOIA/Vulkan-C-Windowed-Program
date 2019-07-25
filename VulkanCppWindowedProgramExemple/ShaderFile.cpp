#include "ShaderFile.h"

std::vector<char> readfile(const std::string & filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error(std::string{ "failde to open file" });
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char>buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
