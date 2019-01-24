#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <algorithm>

using namespace std;

typedef struct _texture_all {
  string varname;
  
  vector<string> texname;
  vector<unsigned short> texlayer;
  
  string ddsfname;
} dds_all;


class EDMReader
{
	FILE *fp;
	unsigned short version;
	vector<string> lookup;

private:
	// Read the v10 lookup table
	void read_lookup()
	{
		unsigned int lookupSize = read_uint();
		stringstream data(read(lookupSize));
		string segment;
		while (getline(data, segment, '\0'))
		{
			lookup.push_back(segment);
		}
	}

public:

	EDMReader(string filename)
	{
		fp = fopen(filename.c_str(), "rb");

		// Validate the header
		string header = string(3, 0);
		fread(&header[0], 1, 3, fp);
		if (header != "EDM") {
			printf("Error: \"%s\" is not a recognised file format\n", header.c_str());
			throw runtime_error("Unrecognised file type");
		}

		version = read_ushort();
		// printf("File version: %d\n", version);

		if (version == 10) {
			// Do v10 version stuff
			read_lookup();
		}
		else if (version != 8) {
			printf("Error: .edm version %d unrecognised\n", version);
			throw runtime_error("Unrecognised file version");
		}
	}

	~EDMReader()
	{
		if (fp != NULL) {
			fclose(fp);
			fp = NULL;
		}
	}

	void seek(long int offset, int origin = SEEK_CUR)
	{
		fseek(fp, offset, origin);
	}

	long int tell()
	{
		return ftell(fp);
	}

	void close()
	{
		fclose(fp);
	}

	string read(size_t length)
	{
		string data(length, 0);
		fread(&data[0], 1, length, fp);
		return data;
	}

	unsigned int read_uint()
	{
		unsigned int value;
		fread(&value, 4, 1, fp);
		return value;
	}

	unsigned int read_ushort()
	{
		unsigned short value;
		fread(&value, 2, 1, fp);
		return value;
	}

	/// Read a uint-prefixed string from a file.
	/// This never uses a lookup table, even if v10
	string read_uint_string()
	{
		// Read the count
		unsigned int size = read_uint();
		if (size > 200) {
			throw runtime_error("Error: String too long");
		}

		// Read the value
		string value(size, 0);
		fread(&value[0], 1, size, fp);
		return value;
	}

	/// Read a string from the file. If v10, use the lookup table
	string read_string()
	{
		if (version == 10) {
			unsigned int index = read_uint();
			return lookup[index];
		}
		else {
			return read_uint_string();
		}
	}

	/// Skip over a propertiesset without fully reading it.
	/// This is so we don't need to work out how to properly represent the
	/// changing property types.
	void skip_propertiesset()
	{
		unsigned int count = read_uint();
		for (unsigned int i = 0; i < count; ++i) {
			string type = read_string();
			string name = read_string();
			// The next field depends on type
			if (type == "model::Property<unsigned int>") {
				fseek(fp, 4, SEEK_CUR);
			}
			else if (type == "model::Property<float>") {
				fseek(fp, 4, SEEK_CUR);
			}
			else if (type == "model::Property<osg::Vec2f>") {
				fseek(fp, 8, SEEK_CUR);
			}
			else if (type == "model::Property<osg::Vec3f>") {
				fseek(fp, 12, SEEK_CUR);
			}
			else if (type == "model::AnimatedProperty<float>") {
				read_uint(); // argument
				unsigned int count = read_uint(); // Frame count
												  // Float = double key + float value = 12 bytes
				seek(count * 12);
			}
			else if (type == "model::AnimatedProperty<osg::Vec2f>") {
				read_uint(); // argument
				unsigned int count = read_uint(); // Frame count
				seek(count * 16);
			}
			else if (type == "model::AnimatedProperty<osg::Vec3f>") {
				read_uint(); // argument
				unsigned int count = read_uint(); // Frame count
				seek(count * 20);
			}
			else if (type == "model::ArgumentProperty") {
				// Slightly unsure on this one, but seems to be a single int. 
				// Assume connects the value to the argument value.
				read_uint();
			}
			else {
				printf("No idea how to read %s\n", type.c_str());
				throw runtime_error("Don't know how to read");
			}
		}
	}

	/// Read an edm string:uint map from a file e.g. the file index
	map<string, unsigned int> read_index()
	{
		map<string, unsigned int> index;
		unsigned int count;
		fread(&count, 4, 1, fp);
		for (unsigned int i = 0; i < count; ++i) {
			string name = read_string();
			unsigned int value = read_uint();
			index[name] = value;
			// printf("Read name %s = %d\n", name.c_str(), value);
		}
		return index;
	}
};

// Convenience for sanity; wraps up the long type
typedef pair<string, map<int, string> > material_textures;

/// Read a material. Returns a map of:
///   Material Name : [Channel : Texture Name]
material_textures read_material(EDMReader &reader)
{
	string name;
	map<int, string> channels;

	// Read every material section
	unsigned int sections = reader.read_uint();
	for (unsigned int i = 0; i < sections; ++i) {
		string sectionName = reader.read_string();
		// How to skip/read the next section, depends on what the section is
		if (sectionName == "BLENDING") {
			reader.seek(1);
		}
		else if (sectionName == "CULLING") {
			reader.seek(1);
		}
		else if (sectionName == "DEPTH_BIAS") {
			reader.read_uint();
		}
		else if (sectionName == "MATERIAL_NAME") {
			reader.read_string();
		}
		else if (sectionName == "NAME") {
			name = reader.read_string();
		}
		else if (sectionName == "SHADOWS") {
			reader.seek(1);
		}
		else if (sectionName == "TEXTURE_COORDINATES_CHANNELS") {
			unsigned int count = reader.read_uint();
			reader.seek(count * 4);
		}
		else if (sectionName == "UNIFORMS") {
			reader.skip_propertiesset();
		}
		else if (sectionName == "ANIMATED_UNIFORMS") {
			reader.skip_propertiesset();
		}
		else if (sectionName == "VERTEX_FORMAT") {
			unsigned int count = reader.read_uint();
			reader.seek(count);
		}
		else if (sectionName == "TEXTURES") {
			unsigned int textureCount = reader.read_uint();
			for (unsigned int t = 0; t < textureCount; ++t) {
				unsigned int index = reader.read_uint();
				reader.read_uint(); // Always -1, as far as observed
				string name = reader.read_string();
				// Skip the rest of the entry - four uints and a matrixf (4+16)*4
				reader.seek(80);
				channels[index] = name;
			}
		}
		else {
			printf("Error: Unrecognised material section %s\n", sectionName.c_str());
			throw runtime_error("Unrecognised material section");
		}
	}

	return make_pair(name, channels);
}

/// Do the extraction process
int edmextract_v10(const char* fn, int savetogether, int advanced) {
	EDMReader reader = EDMReader(string(fn));

	// Read and discard the two indices
	reader.read_index();
	reader.read_index();

	// Next should be the start of model::RootNode
	if (reader.read_string() != "model::RootNode") {
		printf("Unexpected node when expected 'model::RootNode'");
		return 1;
	}
	// Read the rootnode gunk. Normally we'd abstract this as it's common
	// to all node types, but we only want to race to the materials section
	reader.read_uint_string();
	reader.read_uint(); // class version
	reader.skip_propertiesset(); // Class properties
								 // Skip to the start of the materials section, past the bounding boxes
	reader.seek(145, SEEK_CUR);

	// Read the count of the number of materials
	unsigned int material_count = reader.read_uint();
	// Read every material
	vector<material_textures> materials;
	for (unsigned int i = 0; i < material_count; ++i) {
		material_textures mat = read_material(reader);

		// If this material name already exists in the vector, assume that the
		// textures are identical and skip this one
		auto compare = [mat](const material_textures &mc) {return mc.first == mat.first; };
		if (any_of(materials.begin(), materials.end(), compare)) {
			continue;
		}
		materials.push_back(mat);
	}

	// We've now finished reading the file  
	// printf("Finished reading at file position %ld with %d materials\n", reader.tell(), material_count);
	reader.close();


	// OUTPUT


	if (materials.size() == 0) {
		printf("Err: empty\n");
		//return 2;
	}

	char runout[200], fname[200];
	string out_file, temp;
	string::size_type pos = 0, pre_pos = 0;
	temp = fn;
	pos = temp.find_last_of(".");
	pre_pos = temp.find_last_of("\\");
	if (pre_pos>0 && pre_pos<temp.size())
		pre_pos++;
	else
		pre_pos = 0;

	if (savetogether == 0)
		pre_pos = 0;

	out_file = temp.substr(pre_pos, pos - pre_pos);
	strcpy(runout, (char*)out_file.c_str());
	sprintf(fname, "%s.lua", runout);

	FILE *sfp = NULL;
	sfp = fopen(fname, "w");
	if (NULL == sfp) {
		printf("ERROR: fopen failed\n");
		return 0;
	}

	fprintf(sfp, "-- rename it to description.lua\n");
	fprintf(sfp, "livery = \n{\n");
	fprintf(sfp, "    --[[\n");
	fprintf(sfp, "        uncomment lines for customized dds/tga/bmp files\n");
	fprintf(sfp, "    --]]\n");


	char spacetune[64] = "";

	// Dump all of the texture channel information
	for (auto mat : materials) {
		// printf("Material %s\n", mat.first.c_str());
		for (auto texture : mat.second) {
			// printf("  %2d: %s\n", texture.first, texture.second.c_str()); 
			fprintf(sfp, "    --{\"%s\", %d, \"%s\", true};\n", mat.first.c_str(), texture.first, texture.second.c_str());
		}
	}

	fprintf(sfp, "}\n");
	fprintf(sfp, "----== below part is not required for cockpit livery ==----\n");
	fprintf(sfp, "--[[ name your own skin in default language (en)\n");
	fprintf(sfp, "     meanwhile, you can also name the skin in more than one languages,\n     replace xx by [ru, cn, cs, de, es, fr, or it] ]]\n");
	fprintf(sfp, "name = \"\"\n");
	fprintf(sfp, "--name_xx = \"\"\n");
	fprintf(sfp, "--[[ assign the countries\n");
	fprintf(sfp, "     if you want no country limitation,\n");
	fprintf(sfp, "     then comment out below line]]\n");
	fprintf(sfp, "countries = {\"\"}\n");

	fclose(sfp);

	return 1;
}

/*
/// Do the extraction process
int edmextract_v10(const char* fn, int savetogether, int advanced) {
	EDMReader reader = EDMReader(string(fn));

	// Read and discard the two indices
	reader.read_index();
	reader.read_index();

	// Next should be the start of model::RootNode
	if (reader.read_string() != "model::RootNode") {
		printf("Unexpected node when expected 'model::RootNode'");
		return 1;
	}
	// Read the rootnode gunk. Normally we'd abstract this as it's common
	// to all node types, but we only want to race to the materials section
	reader.read_uint_string();
	reader.read_uint(); // class version
	reader.skip_propertiesset(); // Class properties
								 // Skip to the start of the materials section, past the bounding boxes
	reader.seek(145, SEEK_CUR);

	// Read the count of the number of materials
	unsigned int material_count = reader.read_uint();
	// Read every material
	vector<material_textures> materials;
	for (unsigned int i = 0; i < material_count; ++i) {
		material_textures mat = read_material(reader);
		materials.push_back(mat);
	}

	// We've now finished reading the file  
	// printf("Finished reading at file position %ld with %d materials\n", reader.tell(), material_count);
	reader.close();

	// OUTPUT
	if (materials.size() == 0) {
		printf("Err: empty\n");
		//return 2;
	}

	char runout[200], fname[200];
	string out_file, temp;
	string::size_type pos = 0, pre_pos = 0;
	temp = fn;
	pos = temp.find_last_of(".");
	pre_pos = temp.find_last_of("\\");
	if (pre_pos>0 && pre_pos<temp.size())
		pre_pos++;
	else
		pre_pos = 0;

	if (savetogether == 0)
		pre_pos = 0;

	out_file = temp.substr(pre_pos, pos - pre_pos);
	strcpy(runout, (char*)out_file.c_str());
	sprintf(fname, "%s.lua", runout);

	FILE *sfp = NULL;
	sfp = fopen(fname, "w");
	if (NULL == sfp) {
		printf("ERROR: fopen failed\n");
		return 0;
	}

	fprintf(sfp, "-- rename it to description.lua\n");
	fprintf(sfp, "livery = \n{\n");
	fprintf(sfp, "    --[[\n");
	fprintf(sfp, "        uncomment lines for customized dds/tga/bmp files\n");
	fprintf(sfp, "    --]]\n");


	char spacetune[64] = "";

	// Dump all of the texture channel information
	for (auto mat : materials) {
		// printf("Material %s\n", mat.first.c_str());
		for (auto texture : mat.second) {
			// printf("  %2d: %s\n", texture.first, texture.second.c_str()); 
			fprintf(sfp, "    --{\"%s\", %d, \"%s\", true};\n", mat.first.c_str(), texture.first, texture.second.c_str());
		}
	}

	fprintf(sfp, "}\n");
	fprintf(sfp, "----== below part is not required for cockpit livery ==----\n");
	fprintf(sfp, "--[[ name your own skin in default language (en)\n");
	fprintf(sfp, "     meanwhile, you can also name the skin in more than one languages,\n     replace xx by [ru, cn, cs, de, es, fr, ko, or pl] ]]\n");
	fprintf(sfp, "name = \"\"\n");
	fprintf(sfp, "--name_xx = \"\"\n");
	fprintf(sfp, "--[[ assign the countries\n");
	fprintf(sfp, "     if you want no country limitation,\n");
	fprintf(sfp, "     then comment out below line]]\n");
	fprintf(sfp, "countries = {\"\"}\n");

	fclose(sfp);

	return 1;
}
*/

int checkversion(const char* fn) {
	string filename = string(fn);
	FILE* fp = fopen(filename.c_str(), "rb");

	if (fp == NULL) return -1;

	printf("fp: %p\n", fp);
	// Validate the header
	string header = string(3, 0);
	fread(&header[0], 1, 3, fp);
	if (header != "EDM") {
		printf("Error: \"%s\" is not a recognised file format\n", header.c_str());
		//throw runtime_error("Unrecognised file type");
		fclose(fp);
		return -1;
	}
	printf("fp: %p\n", fp);

	unsigned short value;
	fread(&value, 2, 1, fp);
	int version = value;
	printf("File version: %d\n", version);

	printf("fp: %p\n", fp);

	fclose(fp);
	return version;
}

int edmextract(const char* fn, int savetogether, int advanced) {
	int ver = checkversion(fn);
	int res = 0;

	if (ver == 10)
		res = edmextract_v10(fn, savetogether, advanced);

	return res;
}

/*
int main(int argc, char* argv[])
{
  if (argc != 2){
    printf("Usage: %s filename\n", argv[0]);
    return 0;
  }
  int ret = 0;

  ret = edmextract(argv[1], 1, 0);

  return ret;
}
*/


