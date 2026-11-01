#pragma once
#ifndef __MY_OBG_PERCER_H__
#define __MY_OBG_PERCER_H__
#ifndef _GLIBCXX_FSTREAM
	#include <fstream>
#endif
#ifndef _STRING_
#include <string>
#endif
#ifndef _FORWARD_LIST_
#include <forward_list>
#endif

namespace prs {
	class CParser_Obj {
		private:
			struct SPoint {
				float x, y, z;
				SPoint(float a, float b, float c) : x(a), y(b), z(c) {};
				SPoint() : x(0), y(0), z(0) {};
			};
			struct SIndex {
				uint32_t v, t, n;
				SIndex(uint32_t a, uint32_t b, uint32_t c) : v(a), t(b), n(c) {}
				SIndex() : v(0), t(0), n(0) {}
			};
			std::ifstream MyFile; //use all the method of the ifstream
			std::forward_list<std::string> Coord{};
			std::forward_list<SIndex> IndxList{};
			std::forward_list<SPoint> VrtxList{};
			std::forward_list<SPoint> NrmsList{};
			std::forward_list<SPoint> TxtrList{};

			//ѕередача в массив с отчисткой листа! отчистить данные free()
			inline SPoint* move_local(uint32_t* len_array, std::forward_list<SPoint>* local_list) {
				*len_array = std::distance(local_list->begin(), local_list->end());
				uint32_t p = 0;
				SPoint* out_array{};
				out_array = (SPoint*)malloc(*len_array * sizeof(SPoint));
				if (!out_array) { 
					*len_array = 0; 
					return nullptr; 
				}
				for (auto& item : *local_list) {
					out_array[p].x = item.x;
					out_array[p].y = item.y;
					out_array[p].z = item.z;
					++p;
				}
				local_list->clear();
				return out_array;
			}

		public:
			CParser_Obj() {}
			~CParser_Obj() {
				Coord.clear();
				IndxList.clear();
				VrtxList.clear();
				NrmsList.clear();
				TxtrList.clear();
			}

			int32_t load_obj_file(const char* szFileName) {
				MyFile.open(szFileName);
				if (MyFile.is_open()) {
					char buf[256]; //create buffer to read line
					while (!MyFile.eof())//while we are not at the end of the file.
					{
						MyFile.getline(buf, 256);
						Coord.push_front(std::string(buf));
					}
					MyFile.close();
				} 
				else {
					return -1;
				}
				if (!Coord.empty()) {
					uint32_t c{0};
					float		tmp_X, tmp_Y, tmp_Z;
					uint32_t	tmp_v1, tmp_v2, tmp_v3;
					uint32_t	tmp_n1, tmp_n2, tmp_n3;
					uint32_t	tmp_t1, tmp_t2, tmp_t3;
					for (auto& item : Coord) {
						if (item.c_str()[0] != '#') {
							if (item.c_str()[0] == 'v' && item.c_str()[1] == ' '){
								sscanf_s(item.c_str(), "v %f %f %f", &tmp_X, &tmp_Y, &tmp_Z);
								VrtxList.push_front(SPoint(tmp_X, tmp_Y, tmp_Z));
							}
							if (item.c_str()[0] == 'v' && item.c_str()[1] == 'n' && item.c_str()[2] == ' '){
								sscanf_s(item.c_str(), "vn %f %f %f", &tmp_X, &tmp_Y, &tmp_Z);
								NrmsList.push_front(SPoint(tmp_X, tmp_Y, tmp_Z));
							}
							if (item.c_str()[0] == 'v' && item.c_str()[1] == 't' && item.c_str()[2] == ' '){
								sscanf_s(item.c_str(), "vt %f %f", &tmp_X, &tmp_Y);
								TxtrList.push_front(SPoint(tmp_X, tmp_Y, 0));
							}
							else if (item.c_str()[0] == 'f' && item.c_str()[1] == ' ') {
								sscanf_s(item.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i",
									&tmp_v1, &tmp_t1, &tmp_n1,
									&tmp_v2, &tmp_t2, &tmp_n2,
									&tmp_v3, &tmp_t3, &tmp_n3
								);
								IndxList.push_front(SIndex( --tmp_v3, --tmp_t3, --tmp_n3) );
								IndxList.push_front(SIndex( --tmp_v2, --tmp_t2, --tmp_n2) );
								IndxList.push_front(SIndex( --tmp_v1, --tmp_t1, --tmp_n1) );
							}
						}
					}
					Coord.clear();
				}
				return 0;
			}
			//ѕјћя“№ ¬џƒ≈Ћя≈“—я  ј : malloc() !!! данные в объекте удал€ютс€
			float* get_only_vertex_data(size_t* len_array) {
				if (IndxList.empty() || VrtxList.empty()) {
					if(IndxList.empty()) std::cout << "\n IndxList is empty" << std::endl;
					if(VrtxList.empty()) std::cout << "\n VrtxList is empty" << std::endl;
					if(NrmsList.empty()) std::cout << "\n NrmsList is empty" << std::endl;
					if(TxtrList.empty()) std::cout << "\n TxtrList is empty" << std::endl;
					*len_array = 0;
					return nullptr;
				}
				if (NrmsList.empty()) NrmsList.push_front(SPoint());
				if (TxtrList.empty()) TxtrList.push_front(SPoint());

				uint32_t ind_len, nrm_len, vrt_len, txr_len, v, n, t, i = 0, p = 0;
				SPoint* tmp_VertexArray{};
				SPoint* tmp_NormalArray{};
				SPoint* tmp_TexturArray{};
				ind_len = std::distance(IndxList.begin(), IndxList.end());
				tmp_VertexArray = move_local(&vrt_len, &VrtxList);
				tmp_NormalArray = move_local(&nrm_len, &NrmsList);
				tmp_TexturArray = move_local(&txr_len, &TxtrList);
				float* Attribs = (float*)malloc((*len_array = ind_len * 8) * sizeof(float));
				if (!Attribs || !tmp_VertexArray || !tmp_NormalArray || !tmp_TexturArray) {
					if(tmp_VertexArray) free(tmp_VertexArray);
					if(tmp_NormalArray) free(tmp_NormalArray);
					if(tmp_TexturArray) free(tmp_TexturArray);
					*len_array = 0;
					std::cout << "\n Error MEMORY malloc() in get_only_vertex_data(size_t* len_array)" << std::endl;
					return nullptr;
				}
				for (auto& item : IndxList) {
					v = item.v; n = item.n; t = item.t;
					if (p + 7 > *len_array) {
						std::cout << "\n AttribsArray OVERFLOW!!! "<< p+7 << "/" << *len_array << std::endl;
					}
					Attribs[ p ] = tmp_VertexArray[v].x;
					Attribs[++p] = tmp_VertexArray[v].y;
					Attribs[++p] = tmp_VertexArray[v].z;

					Attribs[++p] = tmp_NormalArray[n].x;
					Attribs[++p] = tmp_NormalArray[n].y;
					Attribs[++p] = tmp_NormalArray[n].z;

					Attribs[++p] = tmp_TexturArray[t].x;
					Attribs[++p] = tmp_TexturArray[t].y;
					++p;
				}
				IndxList.clear();
				free(tmp_VertexArray);
				free(tmp_NormalArray);
				free(tmp_TexturArray);
				return Attribs;
			}//-------------------------------------------------------------------------------------------------------------------------------------
	};

}
#endif //__MY_OBG_PERCER_H__