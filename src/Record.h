#pragma once
#include <vector>

class Record {
	public:
		size_t child = 0;
		std::vector<std::pair<int, int> > startCoords;
		bool is_root = false;
		std::vector<size_t> parents;
		int root_length;
		int node_height;
		bool parentsIncludes(size_t o) {
			for (unsigned int i = 0; i < parents.size(); i++) {
				if (parents[i] == o)return true;
			}
			return false;
		}
		Record() {
			child = 0;
			is_root = false;
			parents.clear();
			startCoords.clear();
			root_length = 0;
			node_height = 0;
		}
		Record(const Record & r) {
			this->child = r.child;
			this->is_root = r.is_root;
			this->parents.clear();
			this->startCoords.clear();
			for (unsigned int i = 0; i < r.parents.size(); i++) {
				this->parents.push_back(r.parents[i]);
			}
			for (unsigned int i = 0; i < r.startCoords.size(); i++) {
				this->startCoords.push_back(r.startCoords[i]);
			}
			this->node_height = r.node_height;
			this->root_length = this->parents.size();
		}
};