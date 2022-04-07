#include "grams.h"
#include "util.h"
#include <iostream>
#include <regex>
#include <set>
#include <numeric>
#include <iomanip>
#include <cmath>
#include <map>
#include <cstring>
#include <algorithm>
#include <math.h>
using namespace std;

// define some global variables
vector<string> words;                       // all words, sort by appearance order
vector<pair<string, int>> freq;             // frequency of coocurrence of each word
// for simplicity, each row of the co-occurrence matrix represents the word by lexicographical order
// thus, we'd use a map to store the index of the word which is `word_index`
map<string, int> word_index;                // mapping the word to the index in the co-occurrence matrix
map<int, string> index_word;                // mapping the index to the word in the co-occurrence matrix
vector<vector<int>> coocur_matrix;          // co-ocurrence martix, each row is a n-gram
vector<vector<double>> normalized_matrix;   // normalized co-ocurrence martix
// You can add more global variables here if you need.
// Notice that if you add more global variables, you should declare them in the `grams.h` using `extern`

/**
 * @brief read the input file and record all words in the file. 
 *  Store the words in the vector `words` according to the appearance order.
 * 
 * @param filename  file name of the input file
 * @param words     vector to store the words
 * @return size_t   the size of the `words` vector
 */

size_t record_words(const string& filename, vector<string>& words) {
    ifstream file(filename);
    if (!file.good()) {
        cerr << "Error: cannot open file " << filename << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        split_string(line, words);
    }
    file.close();
    return words.size();
}

/**
 * @brief go through the vector `words` and record the frequency of each word.
 * e.g.:
 *  words = ["a", "a", "b", "c", "d", "e", "e"]
 *  freq = [("a", 2), ("b", 1), ("c", 1), ("d", 1), ("e", 2)]
 * 
 * @param words     vector of words
 * @param freq      vector to store the frequency of each word
 * @return size_t   the size of the `freq` vector
 */
size_t record_freq(const vector<string>& words, vector<pair<string, int>>& freq) {
    // hint: you may find the `map` data structure useful
    // TODO: impelement this function.
    /* Your code here */
    map<string,int> freq_map;
    int words_sz=words.size();
    for(int i=0;i<words_sz;i++)
    {   
        map<string,int>::iterator word_it;
        word_it=freq_map.find(words[i]);
        if(word_it==freq_map.end())
        {
            freq_map[words[i]]=1;
        }
        else
        {
            freq_map[words[i]]++;
        }
    } 
    for(map<string,int>::const_iterator iter=freq_map.begin();iter!=freq_map.end();iter++)
    {
        pair<string,int> temp=*iter;
        freq.push_back(temp);
    }
    return freq.size();
}

/**
 * @brief Filter out the 3000 most frequent words.
 * !!Notice that if a word is not in the 3000 most frequent words, it should be replaced by `<unk>`.
 * 
 * @param freq      vector of words and their frequency
 * @param words     original vector of words. You should change the vector `words` to keep the 3000 most frequent words.
 */
bool cmp(pair<string,int> a,pair<string,int>b)
{
    return a.second>b.second;
}
void filter_words(vector<pair<string, int>>& freq, vector<string>& words) {
    // hint: you can use function `sort` to sort the vector `freq` by modify the compare function.
    // if you want to write a compare function more gracefully, you can try to use `lambda expression`(It's a new feature in C++11.).
    // TODO: implement this function.
    /* Your code here */
    sort(freq.begin(),freq.end(),cmp);
    set<string> words_set;
    int freq_sz=freq.size();
    for(int i=0;i<freq_sz&&i<3000;i++)
    {
        words_set.insert(freq[i].first);
    }
    set<string>::iterator it;
    int words_sz=words.size();
    for(int i=0;i<words_sz;i++)
    {
        it=words_set.find(words[i]);
        if(it==words_set.end())
        {
            words[i]="<unk>";
        }
    }
    return;
}

/**
 * @brief Use `words` vector to construct a word index map. 
 * The key is the word and the value is the position of the word in the alphabetical order.
 * e.g. :
 * words: ["I", "like", "to", "eat", "apple"]
 * word_index: {"I": 0, "apple": 1, "eat": 2, "like": 3, "to": 4}
 * 
 * @param words         vector of words
 * @param word_index    map to store the word index
 * @return size_t       the size of the `word_index` map
 */


size_t set_word_index(const vector<string>& words, map<string, int>& word_index) {
    // TODO: implement this function
    /* Your code here */
    int words_sz=words.size();
    for(int i=0;i<words_sz;i++)
    {
        map<string,int>::iterator it;
        it=word_index.find(words[i]);
        if(it==word_index.end())
        {
            word_index[words[i]]=0;
        }
    }
    int sum=0;
    for(map<string,int>::iterator iter=word_index.begin();iter!=word_index.end();iter++)
    {
        (*iter).second=sum;
        sum++;
    }


    return word_index.size();
}

/**
 * @brief Set up the coocur matrix object. 
 * Each row of the coocur matrix represents the word by lexicographical order. 
 * The value of each element in the co-occurrence matrix is the number of times that two words appear together.
 * e.g. :
 * consider n as 3,
 * words: ["<unk>", "I", "like", "to", "eat", "apple", "<unk>"]
 * coocur_matrix:   (sorted by lexicographical order. Thus, these rows represent the words ["<unk>", "I", "apple", "eat", "like", "to"])
 * coocur_matrix[1][4] == 1 represents that if consider "I" as center word, the number of times that "I" and "like" appear together is 1.
 * [0, 1, 1, 0, 0, 0]
 * [1, 0, 0, 0, 1, 0]
 * [1, 0, 0, 1, 0, 0]
 * [0, 0, 1, 0, 0, 1]
 * [0, 1, 0, 0, 0, 1]
 * [0, 0, 0, 1, 1, 0]
 * 
 * @param words             vector of words
 * @param word_index        map of word index
 * @param coocur_matrix     vector of vector to store the co-occurrence matrix
 * @param n                 hyper-parameter of n-gram
 */
void set_coocur_matrix(const vector<string>& words, const map<string, int>& word_index, vector<vector<int>>& coocur_matrix, int n) {
        coocur_matrix = vector<vector<int>>(word_index.size(), vector<int>(word_index.size(), 0));
        // If you want, you can do the similar thing to print the progress of other parts.
        // TODO: impelement this function. Store the co-occurrence matrix in the `coocur_matrix` vector.
        /* Your code here */
    int word_len=words.size();
    int left=0;
    int right=left+n-1;
    int a1,a2,c,i,j,k;
    map<string,int>::const_iterator it1,it2;
    while(right!=word_len)
    {
        c=(left+right)/2;
        int sum=1;
        a1=c-1;
        a2=c+1;
        while(a1>=0&&sum<=(n-1)/2)
        {   
           
            it1=word_index.find(words[c]);
            i=(*it1).second;
            it2=word_index.find(words[a1]);
            j=(*it2).second;
            coocur_matrix[i][j]++;
            a1--;
            sum++;
        }
        sum=1;
        while(a2<=word_len-1&&sum<=(n-1)/2)
        {   
            it1=word_index.find(words[c]);
            i=(*it1).second;
            it2=word_index.find(words[a2]);
            k=(*it2).second;
            coocur_matrix[i][k]++;
            a2++;
            sum++;
        }
        left++;
        right++;

    }
    
}

/**
 * @brief normalize the co-occurrence matrix by column.
 * e.g.:
 * co-occurrence matrix:
 * [0, 2, 1, 0, 0, 0]
 * [2, 0, 0, 0, 1, 0]
 * [1, 0, 0, 1, 0, 0]
 * [0, 0, 1, 0, 0, 1]
 * [0, 1, 0, 0, 0, 1]
 * [0, 0, 0, 1, 1, 0]
 * normalized matrix:
 * [0, 2/3, 0.5, 0, 0, 0]
 * [2/3, 0, 0, 0, 0.5, 0]
 * [1/3, 0, 0, 0.5, 0, 0]
 * [0, 0, 0.5, 0, 0, 0.5]
 * [0, 1/3, 0, 0, 0, 0.5]
 * [0, 0, 0, 0.5, 0.5, 0]
 * 
 * @param coocur_matrix         co-occurrence martix
 * @param normalized_matrix     normalized co-occurrence martix
 */
void normalize_matrix(const vector<vector<int>>& coocur_matrix, vector<vector<double>>& normalized_matrix) {
    // TODO: implement this function
    /* Your code here */
    int size=coocur_matrix[0].size();
    normalized_matrix = vector<vector<double>>(size, vector<double>(size, 0));
    double sum=0.0;
    
    for(int j=0;j<size;j++)
    {
        for(int i=0;i<size;i++)
        {
            sum+=coocur_matrix[i][j];
        }
        for(int i=0;i<size;i++)
        {
            normalized_matrix[i][j]=coocur_matrix[i][j]/sum;
        }
        sum=0.0;
    }

}

/**
 * @brief save the normalized co-occurrence matrix to a file. The first line should be the words which each row represents.
 * each number is separated by a space.
 * e.g.:
 * a b c d
 * 0.2 0 0.6 0.2
 * 0 0.5 0.5 0
 * 0.6666666666666666 0.3333333333333333 0 0
 * 0 0 0 1
 * 
 * @param filename      file name of the output file
 * @param matrix        normalized co-occurrence martix
 * @param word_index    map of word to index
 */
void save_matrix(const string& filename, const vector<vector<double>>& matrix, const map<string, int>& word_index) {
    // TODO: save the normalized_matrix to file `result.txt`. Notice that the first row should be the words.
    /* Your code here */
    ofstream fp(filename);
    if(!fp.good())
    {
        cout<<"cant open the file"<<endl;
        return;
    }
    else
    {
        int size=word_index.size();
        for(map<string,int>::const_iterator iter=word_index.begin();iter!=word_index.end();iter++)
        {
            fp<<(*iter).first<<" ";
        }
        fp<<endl;
        for(int i=0;i<size;i++)
        {
            for(int j=0;j<size;j++){
                fp<<matrix[i][j]<<" ";
            }
            fp<<endl;
        }
        fp.close();
    }
}

/**
 * @brief restore the normalized co-occurrence matrix from file `filename`.
 * Assume that the file is in the same format as the one in `save_matrix` function.
 * e.g.:
 * 0.2 0 0.6 0.2
 * 0 0.5 0.5 0
 * 0.6666666666666666 0.3333333333333333 0 0
 * 0 0 0 1
 * 
 * @param filename    file name of the input file
 * @param matrix      normalized co-occurrence martix
 * @param index_word  map of index to word
 */
void restore_matrix(const string& filename, vector<vector<double>>& matrix, map<int, string>& index_word) {
    // hint: you can refer to the `set_word_index` function and `split_string` function.
    // TODO: restore the normalized_matrix from file `result.txt`.
    /* Your code here */
    ifstream fp(filename);
    if(!fp.good())
    {
        cerr<<"cant open the file"<<endl;
        return;
    }
    else
    {   
        string word;
        vector<string>words;
        getline(fp,word);
        split_string(word,words);
        int size=words.size();
        for(int i=0;i<size;i++)
        {
            index_word[i]=words[i];
        }
        //cout<<"index_word ok!"<<endl;
        /*for(map<int,string>::iterator iter=index_word.begin();iter!=index_word.end();iter++)
        {
            cout<<(*iter).first<<":"<<(*iter).second<<" ";
        }*/
        string buf;
        int i=0;
        matrix = vector<vector<double>>(size, vector<double>(size, 0));
        while(getline(fp,buf))
        {   
            vector<string> temp;
            split_string(buf,temp);
            for(int j=0;j<size;j++)
            {   
                matrix[i][j]=stod(temp[j]);
            }
            i++;

        }
    }
    return;
}

/**
 * @brief calculate the cosine similarity of word i and word j
 * you can calculate the cosine similarity by using the formula:
 * cosine_similarity = sum(matrix[i][k]*matrix[j][k]) / (sqrt(sum(matrix[i][k]^2)) * sqrt(sum(matrix[j][k]^2)))
 * if you can not understand the formula, please refer to the lab notes.
 * 
 * @param matrix    normalized co-occurrence martix
 * @param i         index of word i
 * @param j         index of word j
 * @return double   cosine similarity of word i and word j
 */
double similarity(const vector<vector<double>>& matrix, size_t i, size_t j) {
    // TODO: implement this function
    /* Your code here */
    double res;
    int size=matrix[0].size();
    double sum1=0.0,sum2=0.0,sum3=0.0;
    for(int k=0;k<size;k++)
    {
        sum1+=matrix[i][k]*matrix[j][k];
        sum2+=pow(matrix[i][k],2);
        sum3+=pow(matrix[j][k],2);
    }
    if(sum2==0||sum3==0)
    {
        res=0;
    }
    res=sum1/(sqrt(sum2)*sqrt(sum3));
    return res == 0 ? 0 : res;
}

/**
 * @brief find the 5 most similar words to the given word. If there are less than 5 words, then simply return all of them.
 * You should sort them by the similarity score in descending order.
 * If they have the same similarity score, then sort them by the alphabetical order.
 * 
 * @param word                  given word
 * @param matrix                normalized co-occurrence martix
 * @param index_word            map of index to word
 * @return vector<string>       vector of 5 most similar words
 */

vector<string> most_similar(const string& word, const vector<vector<double>>& matrix, const map<int, string>& index_word) {
    // TODO: implement this function. 
    /* Your code here */
    map<int,string>::const_iterator it;
    for(it=index_word.begin();it!=index_word.end();it++)
    {   
        if((*it).second==word)
        {
            break;
        }
    }
    if(it==index_word.end())
    {
         cerr << "Error: cannot find word " << word << endl;
        return vector<string>();
    }
    int i=(*it).first;
    int size=matrix[0].size();
    map<double,int,greater<double>>similarity_index;
    for(int j=0;j<size;j++)
    {
        if(j!=i)
        {
            double temp=similarity(matrix,i,j);
            if(temp>=0)
            similarity_index[temp]=j;
    
            
        }
    }
    int sum=0;
    vector<string> ret;
    for(map<double,int>::iterator it1=similarity_index.begin();it1!=similarity_index.end()&&sum<5;it1++)
    {   
        //cout<<(*it1).first<<": "<<(*it1).second<<endl;
        it=index_word.find((*it1).second);
        ret.push_back((*it).second);
        sum++;
    }
   
    return ret;
   
}

/*int main()
{
    string s;
    cout<<"please input s:"<<endl;
    getline(cin,s);
    vector<string> ret;
    ret.push_back("<unk>");
    split_string(s,ret);
    ret.push_back("<unk>");
    map<string,int> word_index;
    vector<pair<string,int>> freq;
    record_freq(ret,freq);
    filter_words(freq,ret);
    size_t wordindex_sz=set_word_index(ret,word_index);
    vector<vector<int>> coocur_matrix;
    vector<vector<double>> normalized_matrix;
    set_coocur_matrix(ret,word_index,coocur_matrix,3);
    normalize_matrix(coocur_matrix,normalized_matrix);
    int matrix_sz=word_index.size();
    cout<<"the words is:"<<endl;
    for(int i=0;i<ret.size();i++)
    {
        cout<<ret[i]<<" ";
    }
    cout<<"the martrix  is:"<<endl;
    for(int i=0;i<matrix_sz;i++)
    {
        for(int j=0;j<matrix_sz;j++)
        {
            cout<<coocur_matrix[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"the normalized martrix  is:"<<endl;
    for(int i=0;i<matrix_sz;i++)
    {
        for(int j=0;j<matrix_sz;j++)
        {
            cout<<normalized_matrix[i][j]<<" ";
        }
        cout<<endl;
    }
    string filename;
    cout<<"please input the file name:"<<endl;
    cin>>filename;
    save_matrix(filename,normalized_matrix,word_index);
    vector<vector<double>> matrix;
    map<int,string> index_word;
    restore_matrix(filename,matrix,index_word);
    cout<<"restore succeed!!"<<endl;
    cout<<"index_word is"<<endl;
    for(map<int,string>::const_iterator it=index_word.begin();it!=index_word.end();it++)
    {
        cout<<(*it).first<<": "<<(*it).second<<endl;
    }
    vector<string> res;
    res=most_similar("apple",matrix,index_word);
    int res_sz=res.size();
    cout<<endl<<"the most similar 5 words are:"<<endl;
    for(int i=0;i<res_sz;i++)
    {
        cout<<res[i]<<endl;
    }
    return 0;
}*/