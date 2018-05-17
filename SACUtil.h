#pragma once
#include <string>
#include <sstream>

#pragma once

#include <string>
#include <sstream>
using namespace std;

#define	CacheSize	200

class CSACUtil
{
public:
	CSACUtil(int nSACLevel = 1);
	~CSACUtil();

public:
	bool Initialize();
	bool Finalize();
	bool Execute(LPCVORWSTR szFilePath, LPCVORWSTR szOutputDir);
	
protected:
	bool Bind(IVORInterpreter* pInterpreter, IVORCore* pCore);

	bool SaveBitmap(LPCVORWSTR szFilePath, LPCVORWSTR szOutputDir);

	bool SaveAnnotFile(LPCVORWSTR szFilePath, LPCVORWSTR szOutputDir);
	VOR_VOID ExportStructureAnnotFile(IVORCore* pVorCore, int nPageNum, VOR_WCHAR* ipOutputPath);
	VOR_VOID ExportLogicInfo(FILE* pLogFile, IVORPageNode* pPageNode);

protected:
	wstring					m_szFilePath;
	CInterpreterFactory*	m_pFactory;
	int						m_nSACLevel;	// [0,ToggleTypePage]
											// [1,TogglePara,defaultValue]
											// [2,ToggleFigure]
											// [3,ToggleTable]
											// [4,ToggleFormula]
											// [5,ToggleTOC]
public:
	vector<int>				m_vecPageNums;
};
