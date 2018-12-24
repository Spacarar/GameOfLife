#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class Pattern {
    private:
        string name;
        vector<pair< int, int> > coordinates;
    public:
        Pattern() {
            name = "Blank pattern";
            coordinates.clear();
        }
        Pattern(string filename) {
            ifstream fin;
            int readNum;
            int temp1, temp2;
            try {
                fin.open(filename);
                getline(fin, this->name);
                fin >> readNum;
                for(int i = 0; i<readNum && !fin.eof(); i++) {
                    fin >> temp1 >> temp2;
                    coordinates.push_back(make_pair(temp1,temp2));
                }
                fin.close();
            } catch(ios_base::failure err) {
                cout << "could not open: " << filename << endl;
                this->name = 'Error pattern',
                this->coordinates.clear();
            }
        }
        Pattern(string name, vector<pair< int, int> > coords) {
            this->name = name;
            this->coordinates.clear();
            for(int i = 0; i < coords.size(); i++ ) {
                this->coordinates.push_back(make_pair(coords[i].first, coords[i].second));
            }
        }
};

class PatternManager {
    private:
        vector<Pattern> patterns;

    public:
        PatternManager();
        PatternManager(string baseFile);
        ~PatternManager();
};