// File: HeightMap.h
// Desc: Corresponding to .hmv file

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include"..\Common\Common.h"

namespace Terrain
{

template <class T>  // Make it a template so that we can use either unsigned int or float type value to store the height value.
class DLLAPI HeightMap
{
private:
	Common::String m_HeightMapName;  // The filename of the heightmap, used for opening file; 扩展名暂定为.hmv，意为Height Map Value，二进制文件

	T* m_pHeightPoints;
	unsigned int m_NumOfHeightPoints;  // 高度点的数目
	unsigned int m_Width;  // 1行有多少个Height Point
	unsigned int m_Height;  // 1列有多少个Height Point

private:
	HeightMap(const HeightMap& hm) {}
	HeightMap& operator = (const HeightMap& hm) { return *this; }

public:
	HeightMap(): m_HeightMapName(), m_pHeightPoints(0), m_NumOfHeightPoints(0), m_Width(0), m_Height(0) {}
	HeightMap(Common::String sName): m_HeightMapName(sName), m_pHeightPoints(0), m_NumOfHeightPoints(0), m_Width(0), m_Height(0) {}
	~HeightMap();

	void GetName(Common::String& sName) const { sName = m_HeightMapName; }
	void SetName(const Common::String& sName) { m_HeightMapName = sName; }
	unsigned int GetNumOfHeightPoints() const { return m_NumOfHeightPoints; }
	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }

	bool IsLoaded() const { return m_pHeightPoints != 0; }
	T GetHeightValue(unsigned int Row, unsigned int Col) const;  // 读取位于第Row行、第Col列的高度数据

	void ReadFile();
};

template<class T>
HeightMap<T>::~HeightMap()
{
	if(m_pHeightPoints)
	{
		delete[] m_pHeightPoints;
		m_pHeightPoints = 0;
	}
}

template <class T>
T HeightMap<T>::GetHeightValue(unsigned int Row, unsigned int Col) const
{
	if(!m_pHeightPoints)
	{
		// Why did the caller forget to call ReadFile() before getting here?
		throw Common::Exception(Common::EL_BAD, "HeightMap<T>::GetHeightValue", "The Heightmap data buffer is not valid yet! Forgot to call HeightMap<T>::ReadFile()?");
	}

	Row = (Row >= m_Height)? (Row % m_Height): Row;  // Rewind back if the index beyond the mark
	Col = (Col >= m_Width)? (Col % m_Width): Col;  // Rewind back if the index beyond the mark; this can help to create a infinite world
	
	return m_pHeightPoints[Row * m_Width + Col];
}

template <class T>
void HeightMap<T>::ReadFile()
{
	if(m_pHeightPoints)
	{
		delete [] m_pHeightPoints;
		m_pHeightPoints = 0;
	}

	Common::ROFile HeightMapFile;
	HeightMapFile.SetFilename(m_HeightMapName.c_str());
	HeightMapFile.Open();

	// Read from the file of heightmap whose format is currently:
	HeightMapFile.ReadBuffer(&m_NumOfHeightPoints, sizeof(m_NumOfHeightPoints));  // This is the type of height point. Currently, it should be 0
	HeightMapFile.ReadBuffer(&m_NumOfHeightPoints, sizeof(m_NumOfHeightPoints));
	HeightMapFile.ReadBuffer(&m_Width, sizeof(m_Width));
	HeightMapFile.ReadBuffer(&m_Height, sizeof(m_Height));

	m_pHeightPoints = new T[m_NumOfHeightPoints];
	if(!m_pHeightPoints)
	{
		throw Common::Exception(Common::EL_BAD, "HeightMap<T>::ReadFile", "Not enough memory! Failed in allocating memory for Height Points buffer");
	}

	HeightMapFile.ReadBuffer(m_pHeightPoints, m_NumOfHeightPoints * sizeof(T));

	HeightMapFile.Close();
}

}  // namespace

#endif