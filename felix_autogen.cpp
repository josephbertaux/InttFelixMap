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
			onl.arm = flx.svr / 4;

			flx_to_onl[flx] = onl;
			onl_to_flx[onl] = flx;

			if(verbose)
			{
				printf("%s\n", buff);
				printf("lyr: %d ldr: %02d arm: %d\n", onl.lyr, onl.ldr, onl.arm);
				printf("svr: %d chn: %02d => lyr: %d ldr: %02d arm: %d\n", flx.svr, flx.chn, flx_to_onl[flx].lyr, flx_to_onl[flx].ldr, flx_to_onl[flx].arm);
				printf("lyr: %d ldr: %02d arm: %d => svr: %d chn: %02d\n", onl.lyr, onl.ldr, onl.arm, onl_to_flx[onl].svr, onl_to_flx[onl].chn);
				printf("\n");
			}
		}

		file.close();
	}

	return 0;
}

int felix_autogen::write()
{
	std::size_t buff_size = 256;
	char buff[buff_size];

	FILE* file = nullptr;

	std::map<struct Flx_s, struct Onl_s>::iterator flx_itr;
	std::map<struct Onl_s, struct Flx_s>::iterator onl_itr;

	snprintf(buff, buff_size, output_file.c_str(), "cc");
	file = fopen(buff, "w");

	fprintf(file, "#include \"InttFelixMap.h\"\n");
	fprintf(file, "int InttFelix::RawDataToOnline(struct Intt::RawData_s const& raw, struct Intt::Online_s& onl)\n");
	fprintf(file, "{\n");

	int prv_svr = -1;
	int prv_chn = -1;
	for(flx_itr = flx_to_onl.begin(); flx_itr != flx_to_onl.end(); ++flx_itr)
	{
		if(flx_itr->first.svr != prv_svr)
		{
			if(prv_svr == -1)fprintf(file, "\tswitch(raw.felix_server) {\n");
			fprintf(file, "\t\tcase %2d:\n", flx_itr->first.svr);
			prv_svr = flx_itr->first.svr;
		}

		if(flx_itr->first.chn != prv_chn)
		{
			if(prv_chn == -1)fprintf(file, "\t\tswitch(raw.felix_channel) {\n");
			fprintf(file, "\t\t\tcase %2d:\n", flx_itr->first.chn);
			prv_chn = flx_itr->first.chn;
		}

		fprintf(file, "\t\t\tonl.lyr = %2d;onl.ldr = %2d;onl.arm = %2d;return 0;\n", flx_itr->second.lyr, flx_itr->second.ldr, flx_itr->second.arm);

		if(prv_chn != 13)continue;
		fprintf(file, "\t\t\tdefault:\n");
		fprintf(file, "\t\t\treturn 1;\n");
		fprintf(file, "\t\t}\n");
		prv_chn = -1;

		if(prv_svr != 7)continue;
		fprintf(file, "\t\tdefault:\n");
		fprintf(file, "\t\treturn 1;\n");
		fprintf(file, "\t}\n");
		prv_svr = -1;
	}
	fprintf(file, "\n");
	fprintf(file, "\treturn 1;\n");
	fprintf(file, "}\n");

	fprintf(file, "\n");

	fprintf(file, "int InttFelix::OnlineToRawData(struct Intt::Online_s const& onl, struct Intt::RawData_s& raw)\n");
	fprintf(file, "{\n");

	int prv_lyr = -1;
	int prv_ldr = -1;
	int prv_arm = -1;
	for(onl_itr = onl_to_flx.begin(); onl_itr != onl_to_flx.end(); ++onl_itr)
	{
		if(onl_itr->first.lyr != prv_lyr)
		{
			if(prv_lyr == -1)fprintf(file, "\tswitch(onl.lyr) {\n");
			fprintf(file, "\t\tcase %2d:\n", onl_itr->first.lyr);
			prv_lyr = onl_itr->first.lyr;
		}

		if(onl_itr->first.ldr != prv_ldr)
		{
			if(prv_ldr == -1)fprintf(file, "\t\tswitch(onl.ldr) {\n");
			fprintf(file, "\t\t\tcase %2d:\n", onl_itr->first.ldr);
			prv_ldr = onl_itr->first.ldr;
		}

		if(onl_itr->first.arm != prv_arm)
		{
			if(prv_arm == -1)fprintf(file, "\t\t\tswitch(onl.arm) {\n");
			fprintf(file, "\t\t\t\tcase %2d:\n", onl_itr->first.arm);
			prv_arm = onl_itr->first.arm;
		}

		fprintf(file, "\t\t\t\traw.felix_server = %2d;raw.felix_channel = %2d;return 0;\n", onl_itr->second.svr, onl_itr->second.chn);

		if(prv_arm != 1)continue;
		fprintf(file, "\t\t\t\tdefault:\n");
		fprintf(file, "\t\t\t\treturn 1;\n");
		fprintf(file, "\t\t\t}\n");
		prv_arm = -1;

		if(prv_ldr != ((prv_lyr < 2) ? 11 : 15))continue;
		fprintf(file, "\t\t\tdefault:\n");
		fprintf(file, "\t\t\treturn 1;\n");
		fprintf(file, "\t\t}\n");
		prv_ldr = -1;

		if(prv_lyr != 3)continue;
		fprintf(file, "\t\tdefault:\n");
		fprintf(file, "\t\treturn 1;\n");
		fprintf(file, "\t}\n");
		prv_lyr = -1;
	}
	fprintf(file, "\n");
	fprintf(file, "\treturn 1;\n");
	fprintf(file, "}");

	fclose(file);

	snprintf(buff, buff_size, output_file.c_str(), "h");
	file = fopen(buff, "w");

	fprintf(file, "#ifndef INTT_FELIX_MAP_H\n");
	fprintf(file, "#define INTT_FELIX_MAP_H\n");
	fprintf(file, "\n");
	fprintf(file, "#include \"InttMapping.h\"\n");
	fprintf(file, "\n");
	fprintf(file, "namespace InttFelix\n");
	fprintf(file, "{\n");
	fprintf(file, "\tint RawDataToOnline(struct Intt::RawData_s const&, struct Intt::Online_s&);\n");
	fprintf(file, "\tint OnlineToRawData(struct Intt::Online_s const&, struct Intt::RawData_s&);\n");
	fprintf(file, "};\n");
	fprintf(file, "\n");
	fprintf(file, "#endif//INTT_FELIX_MAP_H");

	fclose(file);

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
	if(lhs.chn != rhs.chn)return lhs.chn < rhs.chn;

	return false;
}
bool operator>(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs){return rhs < lhs;}
bool operator<=(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs){return !(lhs > rhs);}
bool operator>=(struct felix_autogen::Flx_s const& lhs, struct felix_autogen::Flx_s const& rhs){return !(lhs < rhs);}

//Onl_s operator overloading
bool operator==(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs)
{
	if(lhs.lyr != rhs.lyr)return false;
	if(lhs.ldr != rhs.ldr)return false;
	if(lhs.arm != rhs.arm)return false;

	return true;
}
bool operator!=(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return !(lhs == rhs);}

bool operator<(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs)
{
	if(lhs.lyr != rhs.lyr)return lhs.lyr < rhs.lyr;
	if(lhs.ldr != rhs.ldr)return lhs.ldr < rhs.ldr;
	if(lhs.arm != rhs.arm)return lhs.arm < rhs.arm;

	return false;
}
bool operator>(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return rhs < lhs;}
bool operator<=(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return !(lhs > rhs);}
bool operator>=(struct felix_autogen::Onl_s const& lhs, struct felix_autogen::Onl_s const& rhs){return !(lhs < rhs);}
