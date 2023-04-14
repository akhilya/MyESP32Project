#include "URLDecoder.h"

string urlDecode(const string& encodedText) 
{
	stringstream decodedText;
	for (size_t i = 0; i < encodedText.length(); ++i) 
	{
		if (encodedText[i] == '+')
			decodedText << ' '; 
		else if (encodedText[i] == '%' && i + 2 < encodedText.length()) 
		{
			istringstream hexStream(encodedText.substr(i + 1, 2));
			int hexValue = 0;
			hexStream >> hex >> hexValue;
			if (hexStream.fail()) 
			{
				// Ошибка в декодировании
				throw runtime_error("Failed to decode URL-encoded text");
			}
			decodedText << static_cast<char>(hexValue);
			i += 2;
		} 
		else 
			decodedText << encodedText[i];
	}
	return decodedText.str();
}