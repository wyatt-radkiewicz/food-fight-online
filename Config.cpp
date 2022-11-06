#include "Config.h"

//
// CGameConfig Class Functions
//
// Public Functions //
CConfig::CConfig()
	: m_loaded(false)
{

}
CConfig::~CConfig()
{

}
void CConfig::LoadFromFile(const std::string& path)
{
	if (m_loaded) {
		m_configs.clear();
	}

	m_loaded = true;

	// Open the file
	std::ifstream file(path);

	// Get the configs
	while (!file.eof()) {
		std::string configName;
		file >> configName;

		if (CheckReadError(file))
			return;

		int32_t configVal;
		file >> configVal;

		if (CheckReadError(file))
			return;
		
		// Add it to the configs
		m_configs[configName] = configVal;
	}

	// Close the file
	file.close();
}
int32_t CConfig::GetConfig(const std::string& key) const
{
	if (m_loaded)
		return m_configs.at(key);
	else
		return -1;
}
bool CConfig::FindConfig(const std::string& key) const
{
	if (m_configs.find(key) != m_configs.end())
		return true;
	else
		return false;
}

// Private Functions //
bool CConfig::CheckReadError(std::ifstream& file)
{
	bool failed = file.fail();

	if (failed)
		file.close();

	return failed;
}
