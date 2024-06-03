#include "../inc/Request.hpp"
#include "../inc/initialization.hpp"

void addMethodeInMap(string str, string &RequestStr, std::map<string,string> &RequestMap)
{
	size_t begin;
	size_t end;
	begin = RequestStr.find(str);
	if(begin != string::npos)
	{
		begin += str.size();
		end = RequestStr.find_first_of("\r\n", begin);
		cout << str << " " << end << endl;
		str.pop_back();
		str.pop_back();
		RequestMap[str]= RequestStr.substr(begin, end - begin);
	}
}