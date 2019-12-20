#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
    struct miniSequence{ // Holds a ptr to its source and the starting index of its source
        int genomeIndex;
        int index;
    };
    int m_minSearchLength;
    vector<Genome> genomes;
    // map<string, Genome> genomesByName;
    Trie<miniSequence> miniSequences;
    //Helper function
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
    m_minSearchLength = minSearchLength;
}

herImpl::addGenome(const Genome& genome)
{
    genomes.push_back(genome);
    for(int i = 0; i <= genome.length() - m_minSearchLength; i++){
        string s;
        genome.extract(i, m_minSearchLength, s);
        miniSequence a;
        a.genomeIndex = (int)genomes.size() - 1;
        a.index = i;
        miniSequences.insert(s, a);
    }
}

int GenomeMatcherImpl::minimumSearchLength() const
{
    return m_minSearchLength;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    matches.clear();
    
    if(fragment.length() < m_minSearchLength || minimumLength < m_minSearchLength){ //Bad conditions
        return false;
    }
    
    int length = m_minSearchLength;
    
    vector<miniSequence> initialMatches = miniSequences.find(fragment.substr(0, length), exactMatchOnly);
    //Find all the matches of size m_minSearchLength
    if(initialMatches.size() == 0){
        return false;
    }
    //For each intial match, investigate!
    for(int i = 0; i < initialMatches.size(); i++){
        length = m_minSearchLength;
        int startingPosition = initialMatches[i].index; //Always going to position
        string potentialMatch;
        
        Genome genome = genomes[initialMatches[i].genomeIndex];
        
        bool errorAllowed = true;
        
        //If our error was already used or exactMatchOnly is true
        genome.extract(startingPosition, length, potentialMatch);
        if(potentialMatch != fragment.substr(0, length) || exactMatchOnly)
            errorAllowed = false;
        
        while(true){
            length++;
            //If length is greater than the length of the fragment
            if(length > fragment.length())
                break;
            
            //Compare most recent chars now!
            string lastChar;
            if(!genome.extract(startingPosition + length - 1, 1, lastChar)) //If we have to look out of bounds
                break; //Set lastchar
            string fragmentsLastChar = fragment.substr(length - 1, 1);
            
            if(lastChar != fragmentsLastChar && !errorAllowed){
                break; //If they no longer match and we've already used our error
            }
            if(lastChar != fragmentsLastChar)
                errorAllowed = false; //If its the first time they do not match
        }
        
        if(length - 1 >= minimumLength){//Remember, we only care if it is equal or greater to minimumLength
            //Make the match type
            DNAMatch match;
            match.genomeName = genome.name();
            match.position = startingPosition;
            match.length = length - 1;
            
            matches.push_back(match);
        }
    }
    
    if(matches.size() == 0){
        return false;
    }
    return true;
}

//Compares genomes for order
bool genomeMatchInOrder(const GenomeMatch& a, const GenomeMatch& b){
    if(a.percentMatch == b.percentMatch)
        return  a.genomeName < b.genomeName;
    return a.percentMatch > b.percentMatch;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    results.clear();
    if(fragmentMatchLength < m_minSearchLength)
        return false;
    int s = query.length() / fragmentMatchLength; //Number of sequences we will consider
    
    vector<DNAMatch> matches;
    
    //Collect all matches
    vector<DNAMatch> tempMatches;
    for(int i = 0; i < s; i++){
        string queryFragment;
        query.extract(i * fragmentMatchLength, fragmentMatchLength, queryFragment);
        findGenomesWithThisDNA(queryFragment, fragmentMatchLength, exactMatchOnly, tempMatches);
        matches.insert(matches.end(), tempMatches.begin(), tempMatches.end());
    }
    map<string, int> hitsByName;
    
    //Create a map of unique genome names and the number of hits
    for(int j = 0; j < matches.size(); j++){
        string genomeName = matches[j].genomeName;
        
        if(genomeName != query.name()){ //Don't compare same genome
            map<string, int>::iterator it;
            it = hitsByName.find(genomeName);
            if(it == hitsByName.end()){   //If its the first hit, the genome does not exist yet
                hitsByName[genomeName] = 1;
            }
            else{
                it->second++; //If the genome is already in the list
            }
        }
    }

    for(map<string, int>::iterator it = hitsByName.begin(); it != hitsByName.end(); it++){ //Go through hits map
        double hits = it->second;  //Find number of hits
        double p = 100 * hits / s;   //Percentage
        if(p >= matchPercentThreshold){ //if it meets the threshold
            GenomeMatch g;
            g.genomeName = it->first;  //Make and insert the genome
            g.percentMatch = p;
            results.push_back(g);
        }
    }
    sort(results.begin(), results.end(),genomeMatchInOrder);
    
    if(results.size() == 0){
        return false;
    }
    return true;
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
