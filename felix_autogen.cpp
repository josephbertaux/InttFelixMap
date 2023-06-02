#include "felix_autogen.hpp"

felix_autogen::felix_autogen()
{
	//Do nothing
}

felix_autogen::~felix_autogen()
{
	//Do nothing
}

int felix_autogen::read()
{
	flx_to_onl.clear();
	onl_to_flx.clear();

	std::size_t buff_size = 256;
	char buff[buff_size];

	std::size_t pos;
	std::string line;
	std::fstream file;

	struct Flx_s flx;
	struct Onl_s onl;
	for(flx.svr = 0; flx.svr < 8; ++flx.svr)
	{
		snprintf(buff, buff_size, file_format.c_str(), flx.svr);
		file.open(buff, std::ios::in);

		if(!file.is_open())
		{
			std::cout << "Could not open file:" << std::endl;
			std::cout << "\t" << buff << std::endl;
			std::cout << "Exiting" << std::endl;

			return 1;
		}

		while(file)
		{
			std::getline(file, line);

			while(true)
			{
				pos = line.find("#");
				if(pos == std::string::npos)break;
				line = line.substr(0, pos);
			}

			while(std::isspace(line[0]))
			{
				line = line.substr(1);
			}

			if(line.empty())continue;
			sscanf(line.c_str(), "%d %*s %s", &flx.chn, buff);
			sscanf(buff, "B%dL%d", &onl.lyr, &onl.ldr);
			onl.lyr = 2 * onl.lyr + onl.ldr / 100;
			onl.ldr %= 100;

			flx_to_onl[flx] = onl;
			onl_to_flx[onl] = flx;

			if(verbose)printf("svr: %d chn: %02d => lyr: %d ldr: %02d\n", flx.svr, flx.chn, onl.lyr, onl.ldr);
		}

		file.close();
	}

	return 0;
}

int felix_autogen::write()
{
	std::size_t buff_size = 256;
	char buff[buff_size];

	std::size_t pos;
	std::string line;
	std::fstream file;

	std::map<struct Flx_s, struct Onl_s>::iterator flx_itr;
	std::map<struct Onl_s, struct Flx_s>::iterator onl_itr;

	snprintf(buff, buff_size, output_file.c_str(), "cc");
	file.open(buff, std::ios::out | std::ios::trunc);

	file << "#include \"InttFelixMap.h\"" << std::endl;
	file << std::endl;
	file << "int InttFelix::RawDataToOnline(struct Intt::RawData_s const& raw, struct Intt::Online_s& onl)" << std::endl;
	file << "{" << std::endl;
	for(flx_itr = flx_to_onl.begin(); flx_itr != flx_to_onl.end(); ++flx_itr)
	{
		snprintf(buff, buff_size, "\tif(raw.felix_server == %d && raw.felix_channel == %2d){onl.lyr = %d;onl.ldr = %2d;return 0;}", flx_itr->first.svr, flx_itr->first.chn, flx_itr->second.lyr, flx_itr->second.ldr);
		file << buff << std::endl;
	}
	file << std::endl;
	file << "\treturn 1;" << std::endl;
	file << "}" << std::endl;
	file << std::endl;

	file << "int InttFelix::OnlineToRawData(struct Intt::Online_s const& onl, struct Intt::RawData_s& raw)" << std::endl;
	file << "{" << std::endl;
	for(onl_itr = onl_to_flx.begin(); onl_itr != onl_to_flx.end(); ++onl_itr)
	{
		snprintf(buff, buff_size, "\tif(onl.lyr == %d && onl.ldr == %2d){raw.felix_server = %d;raw.felix_channel = %2d;return 0;}", onl_itr->first.lyr, onl_itr->first.ldr, onl_itr->second.svr, onl_itr->second.chn);
		file << buff << std::endl;
	}
	file << std::endl;
	file << "\treturn 1;" << std::endl;
	file << "}" << std::endl;
	file.close();

	snprintf(buff, buff_size, output_file.c_str(), "h");
	file.open(buff, std::ios::out | std::ios::trunc);

	file << "#ifndef INTT_FELIX_MAP_H" << std::endl;
	file << "#define INTT_FELIX_MAP_H" << std::endl;
	file << std::endl;
	file << "#include \"InttFelixMapping.h\"" << std::endl;
	file << std::endl;
	file << "namespace InttFelix" << std::endl;
	file << "{" << std::endl;
	file << "\tint RawDataToOnline(struct Intt::RawData_s const&, struct Intt::Online_s&);" << std::endl;
	file << "\tint OnlineToRawData(struct Intt::Online_s const&, struct Intt::RawData_s&);" << std::endl;
	file << "};" << std::endl;
	file << std::endl;
	file << "#endif//INTT_FELIX_MAP_H" << std::endl;

	file.close();

	return 0;
}

//Flx_s operator overloading
bool operator==(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs)
{
	if(lhs.svr != rhs.svr)return false;
	if(lhs.chn != rhs.chn)return false;

	return true;
}
bool operator!=(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs){return !(lhs == rhs);}

bool operator<(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs)
{
	if(lhs.svr != rhs.svr)return lhs.svr < rhs.svr;

	return lhs.chn < rhs.chn;
}
bool operator>(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs){return rhs < lhs;}
bool operator<=(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs){return !(lhs > rhs);}
bool operator>=(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs){return !(lhs < rhs);}

//Onl_s operator overloading
bool operator==(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs)
{
	if(lhs.lyr != rhs.lyr)return false;
	if(lhs.ldr != rhs.ldr)return false;

	return true;
}
bool operator!=(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return !(lhs == rhs);}

bool operator<(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs)
{
	if(lhs.lyr != rhs.lyr)return lhs.lyr < rhs.lyr;

	return lhs.ldr < rhs.ldr;
}
bool operator>(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return rhs < lhs;}
bool operator<=(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return !(lhs > rhs);}
bool operator>=(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return !(lhs < rhs);}
