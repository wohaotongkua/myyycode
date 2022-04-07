#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

typedef vector<string> StrVec;




int main()
{


    int n, k;
    cin >> n;
    cin >> k;
    StrVec strvec;
    string x;
    for (int i = 0; i < n; i++)
    {
        cin >> x;
        strvec.push_back(x);
    }


 
    string temp;
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
        {
            if (strvec[j].length() > strvec[j + 1].length())
            {
                temp = strvec[j + 1];
                strvec[j + 1] = strvec[j];
                strvec[j] = temp;
            }

        }
    int mark = 1;
    int i = 0;
    while (i <= n - 2)
    {
        if (mark == k)
        {
            cout << strvec[i] << endl;
        }
        if (strvec[i + 1].length() > strvec[i].length())
        {
            mark++;
        }
        i++;

    }
    if (mark == k)
    {
        cout << strvec[i] << endl;
    }
    system("pause");
    return 0;
}