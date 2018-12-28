#pragma once

#include <vector>

class GridRoot {
	protected:
		size_t root_length;
		std::vector<size_t> root_members;

	public:
		GridRoot() {
			this->root_length = 0;
			this->root_members.clear();
		}

		GridRoot(std::vector<size_t> members) {
			this->root_length = members.size();
			this->root_members.clear();
			for (unsigned int i = 0; i < members.size(); i++) {
				this->root_members.push_back(members[i]);
			}
		}
		
		void clear() {
			root_length = 0;
			root_members.clear();
		}

		int size() {
			return root_length;
		}

		size_t member(unsigned int n) {
			return root_members[n % root_members.size()];
		}

		void set(std::vector<size_t> members) {
			this->root_length = members.size();
		}

		void push(size_t member) {
			this->root_length += 1;
			this->root_members.push_back(member);
		}

		bool includes(size_t member) {
			for (unsigned int i = 0; i < root_members.size(); i++) {
				if (root_members[i] == member) {
					return true;
				}
			}
			return false;
		}

		bool isMatching(GridRoot &g) {
			if (g.root_length != this->root_length) {
				return false;
			}
			for (unsigned int i = 0; i < g.root_members.size(); i++) {
				if (!this->includes(g.root_members[i])) {
					return false;
				}
			}
			return true;
		}

};
