#include <DatabaseAPI/Advanced.hpp>

std::string DB::to_lower(std::string sentence)
{
    std::string new_sentence = "";
    for (int i = 0; i < sentence.length(); i++)
    {
        char ch = sentence[i];
        // cout << ch << endl;
        ch = tolower(ch);
        new_sentence += ch;
    }
    return new_sentence;
}