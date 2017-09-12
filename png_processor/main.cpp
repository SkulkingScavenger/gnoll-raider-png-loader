#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#pragma comment(lib, "User32.lib")



int sprite_count = 0;

class proto_sprite{
public:
	std::string name;
	std::string path;
	int image_count;
	int origin_x;
	int origin_y;

	proto_sprite(std::string n, std::string p, int i, int x = 0, int y = 0){
		name = n;
		path = p;
		image_count = i;
		origin_x = x;
		origin_y = y;
	}

};

std::vector<proto_sprite> get_proto_sprites(std::ifstream &input_file){
	std::vector<proto_sprite> proto_sprites;
	std::string line;
	std::string n;
	std::string p;
	std::string i;
	std::string x;
	std::string y;
	while (input_file >> n >> p >> i >> x >> y){
		proto_sprite temp_spr(n, p, std::stoi(i), std::stoi(x), std::stoi(y));
		proto_sprites.push_back(temp_spr);
	}
	return proto_sprites;
}

void load_sprite_from_png(std::ofstream &output_file, std::string spritename, std::string png_path, int image_count, int x, int y){
	int width, height, bpp;
	unsigned char* png = stbi_load(png_path.c_str(), &width, &height, &bpp, 4);
	int w = width / image_count;
	int h = height;
	int r;
	int g;
	int b;
	int a;
	/*
	Sprite temp_spr(1, 2, 1, images);
	sprites[0] = temp_spr;
	*/
	//write header
	
	//write data
	for (int i = 0; i<image_count; i++){
		output_file << "images.clear();";
		output_file << "GLbyte pixels[" << w*h << "] = {";
		for (int j = 0; j<w*h; j++){
			if (j != 0){ output_file << ","; }
				r = png[i*w*4 + (j/w)*w*image_count*4 + (j%w)*4 + 0];
				b = png[i*w*4 + (j/w)*w*image_count*4 + (j%w)*4 + 1];
				g = png[i*w*4 + (j/w)*w*image_count*4 + (j%w)*4 + 2];
				a = png[i*w*4 + (j/w)*w*image_count*4 + (j%w)*4 + 3];
				output_file << r << "," << g << "," << b << "," << a;
		}
		output_file << "};" << std::endl;
		output_file << "Image temp_img(pixels);" << std::endl;
		output_file << "images.push_back(temp_img);" << std::endl;
	}

	//write footer
	output_file << "Sprite temp_spr("<< image_count << "," << w << "," << h << ", images," << x <<"," << y << ");" << std::endl;
	output_file << "sprites[" << sprite_count << "]=temp_spr;" << std::endl;
	sprite_count++;
	stbi_image_free(png);
}

int main(){
	unsigned int i = 0;
	std::ifstream input_file("sprite_registry.txt");

	if (input_file.is_open()){

		std::vector<proto_sprite> proto_sprites = get_proto_sprites(input_file);

		remove("sprite_enum.h");
		std::ofstream output_file("sprite_enum.h");
		output_file << "#ifndef SPRITE_ENUM_H_";
		output_file << "#define SPRITE_ENUM_H_";
		output_file << "enum sprite_t {";
		for (i = 0; i < proto_sprites.size(); i++){ 
			if (i != 0){output_file << ",";} 
			output_file << proto_sprites[i].name;
		}
		output_file << "};" << std::endl;
		output_file.close();
		output_file << "#endif";
		
		remove("sprite_registry.cpp");
		std::ofstream output_file("sprite_registry.cpp");
		if (output_file.is_open()){

			//write_header
			output_file << "#include <GLFW/glfw3.h>" << std::endl;
			output_file << "#include \"sprite.h\"" << std::endl;
			
			output_file << "Sprite sprites[" << proto_sprites.size() << "];" << std::endl;

			output_file << "void register_sprites(){" << std::endl << "std::vector<Image> images;" << std::endl;


			//write body
			for (i = 0; i < proto_sprites.size(); i++){
				load_sprite_from_png(output_file, proto_sprites[i].name, proto_sprites[i].path, proto_sprites[i].image_count, proto_sprites[i].origin_x, proto_sprites[i].origin_y);
			}

			//write footer
			output_file << "}";

			input_file.close();
			output_file.close();
		}else{
			input_file.close();
			std::cout << "output_file is ruined" << std::endl;
		}
	} else {
		std::cout << "input_file is ruined" << std::endl;
	}

	return 0;
}


