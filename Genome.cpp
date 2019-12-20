#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
    GenomeImpl(const string& nm, const string& sequence);
    GenomeImpl(const GenomeImpl& other);
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    int length() const;
    string name() const;
    bool extract(int position, int length, string& fragment) const;
private:
    string m_name;
    string m_sequence;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
    m_name = nm;
    m_sequence = sequence;
}

GenomeImpl::GenomeImpl(const GenomeImpl &other){
    m_name = other.m_name;
    m_sequence = other.m_sequence;
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes) 
{
    genomes.clear();
    string currentLine = "";
    vector<string> names;
    vector<string> sequences;
    string currentSequence = "";
    bool firstName = true;
    //Assume its formatted perfectly
    while(getline(genomeSource, currentLine)){
        if(currentLine.size() == 0){
            return false;
        }
        if(currentLine[0] == '>'){ //If its a name line
            names.push_back(currentLine.substr(1)); //Add the name
            if(!firstName){ //Add past sequence to the container if its not the first line
                sequences.push_back(currentSequence);
                currentSequence.clear();
            }
            firstName = false;
        }
        else{
            for(int i = 0; i < currentLine.length(); i++){
                currentSequence += currentLine[i];
            }
        }
    }
    sequences.push_back(currentSequence); //Add the remaining sequence

    if(names.size() != sequences.size()){
        return false;
    }
    
    //Check that all names and sequeences are formatted correctly
    for(int i = 0; i < names.size(); i++){
        if(names[i].size() == 0 || sequences[i].size() == 0){
            return false;
        }
        //Make sure sequences are only A,C,T,G, or N
        for(int j = 0; j < sequences[i].size(); j++){
            toupper(sequences[i][j]);
            if(sequences[i][j] != 'A' &&
               sequences[i][j] != 'C' &&
               sequences[i][j] != 'T' &&
               sequences[i][j] != 'G' &&
               sequences[i][j] != 'N' ){
                return false;
                
            }
        }
    }
    
    //Create and place genomes
    for(int i = 0; i < names.size(); i++){
       Genome a(names[i], sequences[i]);
        genomes.push_back(a);
    }
    
    return true;
}

int GenomeImpl::length() const
{
    return (int)m_sequence.size();
}

string GenomeImpl::name() const
{
    return m_name;
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
    if(position < 0 || length < 0)
        return false;
    if((position + length) > m_sequence.size())
        return false;
    fragment = m_sequence.substr(position, length);
    return true;
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes) 
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
