// File: INIFile.cpp
// Desc:

#include"INIFile.h"
#include"WOFile.h"

#include<vector>
using std::vector;

namespace Common
{

bool INIFile::GetINI(const String& sField, const String& sKey, String& sValue)
{
	SetPosition(0);
	
	String sTemp;

	// Go to the field
	while(true)
	{
		if(IsFileEOF())
		{
			return false;
		}

		sTemp = ReadLine();
		sTemp.Trim();

		if(sTemp == ("[" + sField + "]"))
		{
			break;
		}
	};

	// Go to the key
	String::size_type PosOfSep;
	while(true)
	{
		if(IsFileEOF())
		{
			return false;
		}

		sTemp = ReadLine();
		if(sTemp.IfContain("["))
		{
			return false;
		}

		PosOfSep = sTemp.find('=');
		if(PosOfSep == String::npos)  // 连“=”号都没有
		{
			continue;
		}
		// Check out whether the key is exactly the key
		String key = sTemp.substr(0, PosOfSep);
		key.Trim();
		if(key == sKey)
		{
			break;
		}
	};
	
	// Extract out the value
	sValue = sTemp.substr(PosOfSep + 1);
	sValue.Trim();

	return !sValue.empty();
}

void INIFile::SetINI(const String& sField, const String& sKey, const String& sValue)
{
	SetPosition(0);

	vector< String > sv;
	String sTemp;
	while(!IsFileEOF())  // Read all original lines into our vector
	{
		sTemp = ReadLine();
		sTemp.Trim();
		sv.push_back(sTemp);
	}

	std::vector< String >::iterator i, iend;
	i = sv.begin();
	iend = sv.end();
	bool bDone = false;
	for(; i != iend; ++i)
	{
		sTemp = *i;
		if(sTemp == "[" + sField + "]")  // find the field
		{
			for(++i; i != iend; ++i)
			{
				if(i->IfContain("["))  // Touch another field
				{
					sv.insert(i, sKey + " = " + sValue);

					bDone = true;
					break;
				}
				else
				{
					sTemp = *i;

					String::size_type PosOfSep = sTemp.find('=');
					if(PosOfSep != String::npos)
					{
						sTemp = sTemp.substr(0, PosOfSep);
					}

					sTemp.Trim();
					if(sTemp == sKey)  // Find the item, set it!
					{
						i->operator = (sKey + " = " + sValue);

						bDone = true;
						break;
					}
				}
			}

			if(!bDone)
			{
				sv.push_back(sKey + " = " + sValue);
				bDone = true;
			}

			break;
		}
	}

	if(!bDone)
	{
		sv.push_back("[" + sField + "]");
		sv.push_back(sKey + " = " + sValue);

		bDone = true;
	}


	// Now update the ini file...

	Close();  // First, close the ini file cause we open it as read-writable file

	WOFile inif;
	inif.SetFilename(m_sFilename);
	inif.Open();

	for(unsigned int j = 0; j < sv.size() - 1; ++j)
	{
		sTemp = sv[j];
		inif.WriteLine(sTemp);
	}
	sTemp = sv[j];  // The last line
	inif.WriteBuffer((const void*)sTemp.c_str(), sTemp.GetSize());

	inif.Close();

	Open();  // Open the ini file for the following use
}

}  // namespace