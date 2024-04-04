#include "settings.h"

Settings::Settings(const char* filename)
{
	mINI::INIFile file(filename);
	mINI::INIStructure file_ini;
	file.read(file_ini);

	for (auto const& iter : Settings::ini)
	{
		auto const& section = iter.first;
		if (file_ini.has(section))
		{
			auto const& collection = iter.second;
			for (auto const& iter_2 : collection)
			{
				auto const& key = iter_2.first;
				if (file_ini[section].has(key))
				{
					Settings::ini[section][key] = file_ini[section][key];
				}
			}
		}
	}

	file.write(Settings::ini, true);

}

Settings* Settings::Get(const char* filename)
{
	if (!Settings::instance && filename)
	{
		Settings::instance = new Settings(filename);
	}
	return Settings::instance;
}

void Settings::End()
{
	Settings::instance = nullptr;
	delete this;
}