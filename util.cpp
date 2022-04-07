#include "util.h"
// You can include any std header file here.
#include <regex>
#include <string>
#include <iostream>
using namespace std;

/**
 * @brief split a string by the delimiter space aka ` ` and append the result to the back of the vector `ret`.
 * 
 * @param s the string to be splitted
 * @param ret result vector. In this lab, argument `ret` is a global variable 
 *            and is used to store the words according to the appearance order.
 */
bool space(char c)
{
    return isspace(c);
}
bool not_space(char c)
{
    return !isspace(c);
}
void split_string(const string& s, vector<string>& ret) {
    // hint: you can use function `substr` to get a substring of the string `s`
    //       if you know what regular expression is, you can use `regex` to easily split the string `s`
    //       stringstream may be useful, too
    // TODO: implement this function
    /* Your code here */
    string::const_iterator i=s.begin();
    while(i!=s.end())
    {
        i=find_if(i,s.end(),not_space);
        string::const_iterator j=find_if(i,s.end(),space);
        if(i!=s.end())
        {
            ret.push_back(string(i,j));
        }
        i=j;

    }
}

