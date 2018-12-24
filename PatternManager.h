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
                for (int i = 0; i<readNum && !fin.eof(); i++) {
                    fin >> temp1 >> temp2;
                    coordinates.push_back(make_pair(temp1,temp2));
                }
                fin.close();
            } catch(ios_base::failure err) {
                cout << "could not open: " << filename << endl;
                this->name = "Error pattern",
                this->coordinates.clear();
            }
        }
        Pattern(string name, vector<pair< int, int> > coords) {
            this->name = name;
            this->coordinates.clear();
            for (int i = 0; i < coords.size(); i++ ) {
                this->coordinates.push_back(make_pair(coords[i].first, coords[i].second));
            }
        }
        Pattern(const Pattern &p) {
            this->name = p.name;
            this->coordinates.clear();
            for (int i = 0; i < p.coordinates.size(); i++) {
                this->coordinates.push_back(make_pair(p.coordinates[i].first, p.coordinates[i].second));
            }
        }
        void save(string filename) {
            ofstream fout;
            try {
                //FIXME WINDOWS BACKSLASH
                fout.open("patterns\\"+filename+".txt");
                fout << this->name << endl;
                fout << this->coordinates.size() << endl;
                for (int i = 0; i < this->coordinates.size(); i++) {
                    fout << this->coordinates[i].first <<" "<< this->coordinates[i].second << endl;
                }
                fout.close();
            } catch(ofstream::failure &writeError) {
               cout << "could not save pattern to: patterns\\" << filename << ".txt\n";
            }
        }
        string me(){
            return this->name;
        }
        vector<pair<int, int> > coords() {
            return this->coordinates;
        }
};

class PatternManager {
    private:
        vector<Pattern> patterns;

    public:
        PatternManager();
        PatternManager(string baseFile);
        ~PatternManager();

        size_t size();
        
        void writePatternFile(int index, string filename);
        void writeAllPatterns();

        void pushPattern(Pattern pattern);
        void pushPattern(string name, vector<pair<int, int> > coords);

        Pattern getPattern(int index);
        int getPatternIndex(string name);
        void setPattern(int index, Pattern &p);

        void popPattern();
        void popPattern(int index);
        void popPattern(string name);
};