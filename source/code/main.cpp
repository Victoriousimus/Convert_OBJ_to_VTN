#include <stdint.h>
#include <iostream>
#include <string>
#include "pars.h"

inline int32_t move_to_new_file_vert(std::string* s) {
	if ((*s)[s->size() - 3] != 'o' || (*s)[s->size() - 2] != 'b' || (*s)[s->size() - 1] != 'j') {
		std::cout << " > FILE: \"" << (*s) << "\"-- IS NOT < .obj > FORMAT! ... exit" << std::endl;
		system("pause");
		return -1;
	}
	prs::CParser_Obj File_Parser;
	float* VetexData{};
	size_t Vetex_len;

	std::cout << " > FILE:\t" << (*s) << "\t\t";
	if (File_Parser.load_obj_file(s->c_str()) < 0) {
		std::cout << "LOST" << std::endl;
		system("pause");
		return -1;
	}else {
		std::cout << "LOADED" << std::endl;
	}

	VetexData = File_Parser.get_only_vertex_data(&Vetex_len);
	std::cout << " > VertexData:\t\t";
	if( !VetexData || Vetex_len==0) {
		std::cout << "LOST" << std::endl;
		system("pause");
		return -1;
	} else {
		std::cout << "PUSHED" << std::endl;
	}

	int32_t s_size = s->size();
	(*s)[s_size - 3] = 'v';
	(*s)[s_size - 2] = 't';
	(*s)[s_size - 1] = 'n';

	std::string out_file_name = "msh_results";
	for (int i = s_size; i > 0; --i) {
		if ((*s)[i] == '\\' || (*s)[i] == '/') {
			for(; i < s_size; ++i) {
				out_file_name += (*s)[i];
			}
			break;
		}
	}

	std::cout
		<< "\n-------------------------------------------------------------\n\t\t SATART_SAVE\n"
		<< "\n > SAVE:\t" << out_file_name;

	FILE* File_out{};
	fopen_s(&File_out, out_file_name.c_str(), "wb");
	if (!File_out) {
		std::cout << "\t\tFAIL" << std::endl;
		free(VetexData);
		system("pause");
		return -1;
	}
	else {
		fwrite(VetexData, Vetex_len * sizeof(float), 1, File_out);
		fclose(File_out);
		std::cout << "\t\tOK" << std::endl;
		free(VetexData);
		return 0;
	}
}

int32_t command(char(*wrd)[128]) {
	if ((*wrd[0] == 'e' || *wrd[0] == 'E')
		&& (*wrd[1] == 'x' || *wrd[1] == 'X')
		&& (*wrd[2] == 'i' || *wrd[2] == 'I')
		&& (*wrd[3] == 't' || *wrd[3] == 'T')) return -1;

	for (int i = 0; i < 128; ++i) {
		if (*wrd[i] == '-' && *wrd[i + 2] == ' ') {
			if (*wrd[i + 1] == 'i') {
				for (int j = i; j < 124; ++j) {
					*wrd[j] = *wrd[j + 3];
				}
				return 1;
			}
			if (*wrd[i + 1] == 'v') {
				for (int j = i; j < 124; ++j) {
					*wrd[j] = *wrd[j + 3];
				}
				return 2;
			}
		}
	}
	return 0;
}

int main(int argc, char* argv[]) {
	std::cout 
		<< "\n-------------------------------------------------------------\n\t\t SATART_PROGRAM \n"
	<< std::endl;
	char input_file_name[128];
	int32_t cmd{ 0 };
	if (argv[1]) {
		std::string s(argv[1]);
		move_to_new_file_vert(&s);
		s.clear();
	}
	while (true) {
		std::cout << "\n> WRITE FILE NAME: ";
		std::cin >> input_file_name;
		cmd = command(&input_file_name);
		if (cmd < 0) break;
		std::string s(input_file_name);
		system("pause");
		move_to_new_file_vert(&s);
		input_file_name[0] = '\0';
		std::cout << "\n";
		s.clear();
	}
	std::cout << "\n\n\t\t  END_PROGRAM \n";
	system("pause");
	return 0;

}