#ifndef FELIX_AUTOGEN_HPP
#define FELIX_AUTOGEN_HPP

#include <cstdio>

#include <iostream>
#include <fstream>
#include <map>
#include <string>

class felix_autogen
{
public:
	struct Flx_s
	{
		int svr = 0;
		int chn = 0;
	};

	struct Onl_s
	{
		int lyr = 0;
		int ldr = 0;
	};

	bool verbose = false;
	std::string file_format = "txt/intt%d_map.txt";
	std::string output_file = "generated_code/InttFelixMap.%s";

protected:
	std::map<struct Flx_s, struct Onl_s> flx_to_onl;
	std::map<struct Onl_s, struct Flx_s> onl_to_flx;

public:
	felix_autogen();
	~felix_autogen();

	int read();
	int write();
};

bool operator==(struct felix_autogen::Flx_s const&, struct felix_autogen::Flx_s const&);
bool operator!=(struct felix_autogen::Flx_s const&, struct felix_autogen::Flx_s const&);

bool operator<(struct felix_autogen::Flx_s const&, struct felix_autogen::Flx_s const&);
bool operator>(struct felix_autogen::Flx_s const&, struct felix_autogen::Flx_s const&);
bool operator<=(struct felix_autogen::Flx_s const&, struct felix_autogen::Flx_s const&);
bool operator>=(struct felix_autogen::Flx_s const&, struct felix_autogen::Flx_s const&);

bool operator==(struct felix_autogen::Onl_s const&, struct felix_autogen::Onl_s const&);
bool operator!=(struct felix_autogen::Onl_s const&, struct felix_autogen::Onl_s const&);

bool operator<(struct felix_autogen::Onl_s const&, struct felix_autogen::Onl_s const&);
bool operator>(struct felix_autogen::Onl_s const&, struct felix_autogen::Onl_s const&);
bool operator<=(struct felix_autogen::Onl_s const&, struct felix_autogen::Onl_s const&);
bool operator>=(struct felix_autogen::Onl_s const&, struct felix_autogen::Onl_s const&);

#endif//FELIX_AUTOGEN_HPP
