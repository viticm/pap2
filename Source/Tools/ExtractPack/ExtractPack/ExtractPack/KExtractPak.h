#pragma once


class KExtractPak
{
public:
	KExtractPak(void);
	~KExtractPak(void);

	bool Init(const char cszPakFile[], const char cszExtracPath[]);

	bool Run();

private:
	bool ExtractNext();

	bool ExtractPak();
private:
	char m_szPakFile[MAX_PATH];
	char m_szExtractPath[MAX_PATH];
};

