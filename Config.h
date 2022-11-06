#pragma once

#include <fstream>
#include <string>
#include <map>

//
// Overview: A game config that contains things such as,
// screen width, height, other options, player settings and speed, ect.
//
class CConfig
{
public:
	CConfig();
	~CConfig();

	void LoadFromFile(const std::string& path);
	int32_t GetConfig(const std::string& key) const;
	bool FindConfig(const std::string& key) const;

private:
	bool CheckReadError(std::ifstream& file);

private:
	std::map<std::string, int32_t> m_configs;

	bool m_loaded;
};
