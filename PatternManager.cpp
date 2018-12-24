#include "PatternManager.h"

PatternManager::PatternManager() {
    this->patterns.clear();
}
PatternManager::PatternManager(string baseFile) {
    string filename;
    ifstream fin;
    try {
        fin.open(baseFile);
        Pattern temp;
        while(fin.good()) {
            getline(fin, filename);
            temp = Pattern(filename);
            if(temp.me() != "Error pattern") {
                this->patterns.push_back(temp);
                cout << "read: "<< temp.me() <<endl;
            }
        }
    } catch(ifstream::failure &readError) {
        cout << "could not create pattern managaer from: " << baseFile << endl;
        this->patterns.clear();
    }

}
PatternManager::~PatternManager() {
    //do nothing
}

size_t PatternManager::size() {
    return this->patterns.size();
}

void PatternManager::writePatternFile(int index, string filename) {
    if (index >= 0 && index < this->patterns.size()){
        this->patterns[index].save(filename);
    }
}
void PatternManager::writeAllPatterns() {
    for (int i = 0; i < this->patterns.size(); i++) {
        this->patterns[i].save(this->patterns[i].me());
    }
}
void PatternManager::pushPattern(Pattern pattern) {
    this->patterns.push_back(pattern);
}
void PatternManager::pushPattern(string name, vector<pair<int, int> > coords) {
    this->patterns.push_back(Pattern(name,coords));
}

Pattern PatternManager::getPattern(int index) {
    if (index >=0 && index < this->patterns.size()) {
        return this->patterns[index];
    } else {
        return Pattern();
    }
}

int PatternManager::getPatternIndex(string name) {
    for (int i = 0; i < this->patterns.size(); i++) {
        if(this->patterns[i].me() == name){
            return i;
        }
    }
    return -1;
}
void PatternManager::setPattern(int index, Pattern &p) {
    if(index >=0 && index < this->patterns.size()) {
        this->patterns[index] = p;
    }
}

void PatternManager::popPattern() {
    if(this->patterns.size()){
        this->patterns.pop_back();
    }
}
void PatternManager::popPattern(int index) {
    if(this->patterns.size()) {
        int count = 0;
        for( vector<Pattern>::iterator it = this->patterns.begin(); it != this->patterns.end(); it++){
            if(count == index){
                this->patterns.erase(it);
            }
            count++;
        }
    }
}
void PatternManager::popPattern(string name) {
    if(this->patterns.size()) {
        for( vector<Pattern>::iterator it = this->patterns.begin(); it != this->patterns.end(); it++){
            if(it->me() == name){
                this->patterns.erase(it);
            }
        }
    }
}