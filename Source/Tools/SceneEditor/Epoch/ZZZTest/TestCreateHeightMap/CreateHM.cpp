// File: CreateHM.cpp
// Desc:

#include"..\..\Common\Common.h"
#include<iostream.h>
#include<windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


Common::String sPath;

void PrintUsage()
{
	cout<<endl;
	cout<<"Usage:"<<endl;
	cout<<endl;
	cout<<"CreateHM.exe [Filename]"<<endl;
	cout<<endl;
	cout<<"  Filename\tThis can be a single filename or a file folder name."<<endl;
	cout<<endl;
}

Common::String TranslateCT(unsigned int i)
{
	Common::String sRet;

	switch(i)
	{
	case BI_RGB:
		sRet = "RGB uncompressed";
		break;

	case BI_RLE8:
		sRet = "RLE8 compressed";
		break;

	case BI_RLE4:
		sRet = "RLE4 compressed";
		break;

	case BI_BITFIELDS:
		sRet = "BITFIELDS uncompressed";
		break;

	default:
		sRet = "Unknown";
		break;
	}

	return sRet;
}

bool ProcessFile(const Common::String& sFilename)
{
	Common::ROFile rof;
	rof.SetFilename(sFilename);
	rof.Open();

	BITMAPFILEHEADER bmpFH;
	BITMAPINFOHEADER bmpIH;
	int SizeOfColorTable=0;

	cout<<endl;
	cout<<"Reading file header..."<<endl;
	rof.ReadBuffer((void *)&bmpFH, sizeof(bmpFH));  //读取文件头
	cout<<"File type: "<<bmpFH.bfType<<endl;
	cout<<"File size: "<<bmpFH.bfSize<<" bytes"<<endl;
	rof.SetPosition(0x000E);
	cout<<"Reading infomation header..."<<endl;
	rof.ReadBuffer((void *)&bmpIH, sizeof(bmpIH));  //读入信息头
	cout<<"Image width: "<<bmpIH.biWidth<<endl;
	cout<<"Image height: "<<bmpIH.biHeight<<endl;
	cout<<"Pixel format: "<<bmpIH.biBitCount<<" bits-per-pixel"<<endl;
	cout<<"Compression type: "<<TranslateCT(bmpIH.biCompression).c_str()<<endl;
	if(bmpIH.biSizeImage == 0)
	{
		bmpIH.biSizeImage = bmpIH.biHeight*((bmpIH.biWidth*bmpIH.biBitCount)/8);
	}
	cout<<"Size of image: "<<bmpIH.biSizeImage<<" bytes"<<endl;
	cout<<"PPM in X axis: "<<bmpIH.biXPelsPerMeter<<" pixels-per-meter"<<endl;
	cout<<"PPM in Y axis: "<<bmpIH.biYPelsPerMeter<<" pixels-per-meter"<<endl;

	unsigned char* m_pHM = new unsigned char[bmpIH.biSizeImage];
	if(!m_pHM)
	{
		delete[] m_pHM;
		return false;
	}

	rof.SetPosition(bmpFH.bfOffBits);
	rof.ReadBuffer((void *)m_pHM, bmpIH.biSizeImage);  //读取图像区

	//Close the handle
	rof.Close();


	Common::WOFile wof;
	wof.SetFilename(sFilename.substr(0, sFilename.find_last_of(".")) + ".hmv");
	wof.Open();

	unsigned int ui = 0;  // Means char
	wof.WriteBuffer((void*)&ui, sizeof(ui));
	ui = bmpIH.biSizeImage;  // The number of height points
	wof.WriteBuffer((void*)&ui, sizeof(ui));
	ui = bmpIH.biWidth;  // The number of height points in a row
	wof.WriteBuffer((void*)&ui, sizeof(ui));
	ui = bmpIH.biHeight;  // The number of height points in a col
	wof.WriteBuffer((void*)&ui, sizeof(ui));
	wof.WriteBuffer((void*)m_pHM, bmpIH.biSizeImage);

	wof.Close();

	return true;
}

bool ProcessFileFolder(const Common::String& sFilename)
{
	cout<<endl;
	cout<<"Folder not support!"<<endl;
	cout<<endl;
	return false;
}

int main(int argc, char* argv[], char* envp[])
{
	try
	{
	if(argc <= 1 || argc > 2)
	{
		PrintUsage();

		return 1;
	}
	else
	{
		sPath = argv[0];
		sPath = sPath.substr(0, sPath.find_last_of("\\") + 1);
		Common::String sFilename = argv[1];

		if(sFilename.IfContain("."))
		{
			if(!ProcessFile(sPath + sFilename))
			{
				return 2;
			}
		}
		else
		{
			if(!ProcessFileFolder(sPath + sFilename))
			{
				return 3;
			}
		}
	}
	}
	catch(Common::Exception e)
	{
		MessageBox(0, e.GetErrorMessage(), e.GetTitle(), MB_OK);

		return 3;
	}

	cout<<endl<<"The job has been finished successfully!"<<endl;

	return 0;
}