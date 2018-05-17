#include "StdAfx.h"
#include "SACUtil.h"

#include "KernelWrapper.h"

#include "IVORPageNode.h"
#include "IVORPageNodeIterator.h"

#include "shlwapi.h"


CSACUtil::CSACUtil(int nSACLevel)
	:m_szFilePath(_T("")),m_nSACLevel(nSACLevel)
{
	m_pFactory = NULL;
}

CSACUtil::~CSACUtil(void)
{
	VOR_DELETE_POINTER(m_pFactory);
	m_vecPageNums.clear();
}

bool CSACUtil::Initialize()
{
	if (NULL == m_pFactory)
	{
		m_pFactory = new CInterpreterFactory();
	}
	
	// 初始化文档引擎
	Init();

	return true;
}

bool CSACUtil::Finalize()
{
	//释放文档引擎
	Destroy();

	return true;
}

bool CSACUtil::Bind(IVORInterpreter* pInterpreter, IVORCore* pCore)
{
	pCore->Unbind();

	if (pCore->Bind(pInterpreter) != 0)
		return false;

	IVORStrategy* pStrategy = CreateStrategy();
	if (pStrategy == NULL)
		return false;

	int nSACLevel = m_nSACLevel;

	switch (nSACLevel)
	{
	case 0:
		pStrategy->ToggleTypePage();
		break;
	case 1:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
		}
		break;
	case 2:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
			pStrategy->ToggleFigure();
		}
		break;
	case 3:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
			pStrategy->ToggleFigure();
			pStrategy->ToggleTable();
		}
		break;
	case 4:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
			pStrategy->ToggleFigure();
			pStrategy->ToggleTable();
			pStrategy->ToggleFormula();
		}
		break;
	case 5:
		pStrategy->ToggleTOC();
		break;
	default:
		break;
	}

	if (pCore->SetPageStrategy(pStrategy) != 0)
		return false;
	DeleteStrategy(pStrategy);

	pCore->SetCacheSize(CacheSize);

	return true;
}

bool CSACUtil::Execute(LPCVORWSTR szFilePath, LPCVORWSTR szOutputDir)
{
	if (100 == m_nSACLevel)			// ExportPageImage
	{
		return SaveBitmap(szFilePath, szOutputDir);
	}
	else
	{
		return SaveAnnotFile(szFilePath, szOutputDir);
	}
}

bool CSACUtil::SaveBitmap(LPCVORWSTR szFilePath, LPCVORWSTR szOutputDir)
{
	if (!PathFileExistsW(szFilePath))
		return false;

	wstring wsFilePath = szFilePath;
	int nStartPos = wsFilePath.rfind('\\');
	int nEndPos = wsFilePath.size();
	wstring wsFileName = wsFilePath.substr(nStartPos, nEndPos-nStartPos);

	// 打开文件
	IVORInterpreter* pInterpreter = m_pFactory->CreateInterpreter(wsFilePath.c_str());
	if (NULL == pInterpreter)
		return false;

	OpenDoc(wsFilePath.c_str());

	//输出图片
	int nPageCount = pInterpreter->InterpretPageCount();
	for (int iPage = 1; iPage < nPageCount+1; iPage++)
	{
		wstring wsOutputPath;
		wstringstream sstream;
		sstream << szOutputDir;
		sstream << wsFileName;
		sstream << "_";
		sstream << iPage;
		sstream <<".png";
		wsOutputPath = sstream.str();

		ExportPageBitmap(iPage, wsOutputPath.c_str());
	}

	//释放资源
	CloseDoc();
	m_pFactory->DeleteInterpreter(pInterpreter);

	return true;
}

bool CSACUtil::SaveAnnotFile(LPCVORWSTR szFilePath, LPCVORWSTR szOutputDir)
{
	if (!PathFileExistsW(szFilePath))
		return false;

	wstring wsFilePath = szFilePath;
	int nStartPos = wsFilePath.rfind('\\');
	int nEndPos = wsFilePath.size();
	wstring wsFileName = wsFilePath.substr(nStartPos, nEndPos-nStartPos);

	// 打开文件
	IVORInterpreter* pInterpreter = m_pFactory->CreateInterpreter(wsFilePath.c_str());
	if (NULL == pInterpreter)
		return false;

	OpenDoc(wsFilePath.c_str());

	IVORCore* pCore = CreateVorCore();
	if (pCore == NULL)
		return false;
	bool bRet = Bind(pInterpreter, pCore);

	//输出识别结果
	int nPageCount = pInterpreter->InterpretPageCount();
	for (int iPage = 1; iPage < nPageCount+1; iPage++)
	{
		if (VORR_OK != pCore->AnalysePage(iPage))
			continue;

		wstring wsOutputPath;
		wstringstream sstream;
		sstream << szOutputDir;
		sstream << wsFileName;
		sstream << "_";
		sstream << iPage;
		sstream <<".xml";
		wsOutputPath = sstream.str();

		ExportStructureAnnotFile(pCore, iPage, (VOR_WCHAR*)wsOutputPath.c_str());
	}

	//释放资源
	pCore->Unbind();
	DeleteVorCore(pCore);
	pCore = NULL;

	CloseDoc();
	m_pFactory->DeleteInterpreter(pInterpreter);

	return true;
}

VOR_VOID CSACUtil::ExportStructureAnnotFile(IVORCore* pVorCore, int nPageNum, VOR_WCHAR* ipOutputPath)
{
	if (NULL == pVorCore)
	{
		return;
	}

	IVORPageNode* pNode = NULL;
	pVorCore->GetPageRootNode(nPageNum, pNode);

	if (NULL == pNode)
	{
		return;
	}

	ContentType aContentTypes[] = {CONTENT_TYPE_PARA, CONTENT_TYPE_FIGURE, CONTENT_TYPE_FORMULA, CONTENT_TYPE_TABLE};

	std::wstring strFileFullName = PathFindFileName(ipOutputPath);

	FILE* pLogFile = _wfopen(ipOutputPath, L"w");
	fprintf(pLogFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fwprintf(pLogFile, L"<document filename=\"%s\">\n", strFileFullName.c_str());
	ExportLogicInfo(pLogFile, pNode);
	fwprintf(pLogFile, L"</document>");
	fclose(pLogFile);
}

VOR_VOID CSACUtil::ExportLogicInfo(FILE* pLogFile, IVORPageNode* pPageNode)
{
	IVORPageNodeIterator* pPageNodeIterator = VOR_NULL;
	pPageNode->GetChildNodes(pPageNodeIterator);

	while(VOR_NULL != pPageNodeIterator && pPageNodeIterator->HasNext())
	{
		double dScale = 4.0f;

		IVORPageNode* pNode = VOR_NULL;
		pPageNodeIterator->Next(pNode);

		VOR_DRECT rcBBox = pNode->GetBBox();
		if (fabsf(rcBBox.dLeft - DBL_MAX) < VOR_EPSILON || fabsf(rcBBox.dBottom - DBL_MAX) < VOR_EPSILON)
		{
			rcBBox.dLeft = 0.0f;
			rcBBox.dBottom = 0.0f;
		}
		rcBBox = VOR_DRECT(rcBBox.dLeft*dScale, rcBBox.dTop*dScale, rcBBox.dRight*dScale, rcBBox.dBottom*dScale);

		if (pNode->GetType() == CONTENT_TYPE_TEXTLINE){
			fprintf(pLogFile, "<textlineRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</textlineRegion>\n");
		}
		else if (pNode->GetType() == CONTENT_TYPE_FIGURE || pNode->GetType() == CONTENT_TYPE_IMAGE) {
			fprintf(pLogFile, "<figureRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</figureRegion>\n");
		}
		else if (pNode->GetType() == CONTENT_TYPE_TABLE) {
			fprintf(pLogFile, "<tableRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</tableRegion>\n");
		}
		else if (pNode->GetType() == CONTENT_TYPE_FORMULA) {
			fprintf(pLogFile, "<formulaRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</formulaRegion>\n");
		}
#if 0	// POD Annotation No Use
		else if (pNode->GetType() == CONTENT_TYPE_FORMULA_EM) {
			fprintf(pLogFile, "<formulaRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</formulaRegion>\n");
		}
#endif
		else {
			ExportLogicInfo(pLogFile, pNode);
		}
	}
}
