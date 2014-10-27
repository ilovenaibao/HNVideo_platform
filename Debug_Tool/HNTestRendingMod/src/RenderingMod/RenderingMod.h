#ifndef _RENDERING_MOD_H_
#define _RENDERING_MOD_H_

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
#include "ITCPModelDataProc.h"
#include "IRenderingComponent.h"
#include "RenderObject.h"
#include "AviWriter.h"
#include "GdiPlusDraw.h"
#include <map>
#include <fstream>
//#include <afxmt.h>
//#include "afxmt.h"//ͬ������ʱ��Ҫ������ͷ�ļ�

#include "data_protocol.h"

#include "Draw.h"

using namespace std;


typedef BOOL (*InitDecoderFunc)(HANDLE *hDecoder);
typedef BOOL (*DecodeStreamFunc)(HANDLE hDecoder, unsigned char* in_buf, int encode_size, int* pWidth, int* pHeight);
typedef BOOL (*GetYUVStreamFunc)(HANDLE hDecoder, unsigned char* out_y, unsigned char* out_u, unsigned char* out_v);
typedef void (*CloseDecoderFunc)(HANDLE *hDecoder);


/**
* @brief	��Ƶ����ģ����
* @remark	���಻Ӧ�ö�̬���������٣���Ϊ���������߳�ǰ��������ʵ���Ѵ����������߳���Init����������ʱѡ��ʱ������
*/
class CRenderingMod : public ITCPModelDataProc
{
public:
	CRenderingMod(void);
	~CRenderingMod(void);

public:
	/**
	* @brief	��ʼ��ʵ��
	* @param[in]	hWnd��ģ����Ҫ���ƵĴ���
	* @param[in]	hContainer�������������ھ��
	* @return	��ʼ���Ƿ�ɹ�
	* @remark	�������̲߳���ȫ������Ϊfalseʱ����Ҫ����Clear�ͷ���Դ
	*/
	bool Init(HWND hWnd, HWND hContainer);

	/**
	* @brief	�ͷ�ʹ�õ���Դ
	* @remark	�������̲߳���ȫ
	*/
	void Clear(void);

	/**
	* @brief	��ʼִ�л��ƹ���
	* @return	�Ƿ�ɹ���ʼ���ƹ���
	* @remark	�����������������ƹ���������false
	*			�������̲߳���ȫ
	*/
	bool Start(void);

	/**
	* @brief	ֹͣ���ƹ���
	* @return	������Ҫ�ȴ��������߳̾�����߳̾���Ĺر��ɵ�����ִ��
	* @remark	�������̲߳���ȫ
	*/
	void Stop(void);


	int	GetDwID(void)
	{
		return dwID;
	}
	/**
	* @brief	������Ƶ֡����
	* @param[in]	nFrameNo��֡���
	* @param[in]	nType����Ƶ֡����
	* @param[in]	lpData����Ƶ֡����
	* @param[in]	nLen����Ƶ���ݳ���
	*/
	bool ProcFrameData(unsigned int nFrameNo, unsigned int nType, LPBYTE lpData, unsigned int nLen);

	bool ProcFrameData_YUV(char *p_buf);

	///**
	//* @brief	����ʵʱ��������
	//* @param[in]	nFrameNo��֡���
	//* @param[in]	nIn��������
	//* @param[in]	nOut��������
	//*/
	//void ProcPeopleCount(unsigned int nFrameNo, unsigned short nIn, unsigned short nOut);

	/**
	* @brief	����ʵʱĿ����������
	* @param[in]	nFrameNo��֡���
	* @param[in]	vObjInfo��Ŀ��������������
	* @param[in]	nLen�����鳤��
	*/
	void ProcObjectLocation(unsigned int nFrameNo, CObjLocInfo* vObjInfo, unsigned int nLen);

	/**
	* @brief	����ʵʱ�豸ʱ��
	* @param[in]	nFrameNo��֡���
	* @param[in]	nHour��Сʱ
	* @param[in]	nMinute������
	* @param[in]	nSecond������
	* @param[in]	nTenMillisecond���ٷ�֮һ��
	*/
	void ProcDevTime(unsigned int nFrameNo, unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenMillisecond);

	/**
	* @brief	����ʵʱ�㷨����ʱ��
	* @param[in]	nFrameNo��֡���
	* @param[in]	nMillisecond��������
	*/
	void ProcAlgoWorkingTime(unsigned int nFrameNo, unsigned short nMillisecond);

	/**
	* @brief	���ÿ���ͳ��������Ϣ���մ���
	* @param[in]	hWnd���������ͳ��������Ϣ�Ĵ��ڵľ��
	*/
	void SetPCWnd(HWND hWnd);

	/**
	* @brief	������Ҫ������ƹ��̵�ģ��
	* @param[in]	compoment��ָ��ʵ���˽ӿ�IRenderingCompoment����ʵ��
	*/
	void SetRenderingProc(IRenderingComponent* compoment);

	/**
	* @brief	���û��ƹ���
	*/
	void ResetRenderingProc(void);

	///**
	//* @brief	�����Ƿ����5���ӿ�������
	//* @param[in]	bShow��trueΪ��ʾ��falseΪ����
	//*/
	//void ShowFiveMinutePeopleCount(bool bShow);

    void ProcAllCounterData(unsigned int nFrameNo, const char* allCounterData, int nlen);

	/**
	* @brief	�����Ƿ����ʵʱ�豸ϵͳʱ��
	* @param[in]	bShow��trueΪ��ʾ��falseΪ����
	*/
	void ShowSysTime(bool bShow);

	/**
	* @brief	�����Ƿ���Ƹ�����
	* @param[in]	bShow��trueΪ��ʾ��falseΪ����
	*/
	void ShowTrajectory(bool bShow);

	/**
	* @brief	�����Ƿ���Ƹ��ٶ���ID
	* @param[in]	bShow��trueΪ��ʾ��falseΪ����
	*/
	void ShowObjectID(bool bShow);

	/**
	* @brief	�����Ƿ�����㷨Ч��
	* @param[in]	bShow��trueΪ��ʾ��falseΪ����
	*/
	void ShowAlgoET(bool bShow);

	/**
	* @brief	��ȡ��Ҫ���ƵĴ��ھ��
	* @return	��Ƶ���ƵĴ��ھ��
	*/
	HWND GetHWnd(void);

	/**
	* @brief	��ʼ¼��
	* @param[in]	lpFilePath��¼���ļ�·�����ַ������һ���ַ�����Ϊ'\0'
	* @return	�Ƿ�ɹ���ʼ¼��
	* @remark	���������lpFilePath�Ƿ�Ϸ�����Ҫ�ɵ����߽�����֤
	*/
	bool StartRecord(LPCTSTR lpFilePath);

	/**
	* @brief	ֹͣ¼��
	*/
	void StopRecord(void);

	/**
	* @brief	������Բ��
	* @details	�˷�������IRenderingComponent::OnRenderingProc�����е��ã����򲻱�֤�̰߳�ȫ
	*/
	static void DrawCircle(DWORD dwId, int xCenter, int yCenter, int nRadius, int nPixel, DWORD FrameColor);

	/**
	* @brief	���ƾ���
	* @details	�˷�������IRenderingComponent::OnRenderingProc�����е��ã����򲻱�֤�̰߳�ȫ
	*/
	static void DrawRectangle(DWORD dwId, int xCenter, int yCenter, int nLenOfSide, int nPixel, DWORD FrameColor);

	/**
	* @brief	�����߼���
	* @details	�˷�������IRenderingComponent::OnRenderingProc�����е��ã����򲻱�֤�̰߳�ȫ
	*/
	static void DrawLines(DWORD dwId, const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor);

    static void DrawTimingPassFlow(int dwID, int wndWidth, int wndHeight, int inPC, int outPC);
    static void DrawDelayInfo(int dwID, int wndWidth, int wndHeight, int delayNum, int maxtime, int totaltime);
    static void DrawTotalEventInfo(int dwID, int wndWidth, int wndHeight, int inNum, int outNum);
    //static void DrawIDTrack(int dwID, int wndWidth, int wndHeight,VECT_TRACK vObj, bool isRetrack);
    static void DrawTimingCross(int dwID, int wndWidth, int wndHeight, int crossNum);
    //static std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>> counterTrack;	// �����ߵĵ㼯��
    //static void ClearCounterTrack(void);

protected:
	/**
	* @brief	���ƹ����߳�
	* @param[in]	obj���̵߳Ĳ�������Աparam
	* @return	�̺߳�������ֵ
	*/
	static unsigned int _stdcall RenderingThreadFunc(void* obj);

	/**
	* @brief	ʵ���̹߳�������
	*/
	void DoWork(void);

	/**
	* @brief	��ʾ��Ƶͼ
	* @param[in]	nID������ģ��ID
	*/
	void Present(void);

	/**
	* @brief	׼�����ڻ��Ƶ�֡����
	*/
	void PrepareRender(void);

	/**
	* @brief	������Ƶ֡
	*/
	void RenderVideoFrame(void);

	//void RenderPassCountToDlg(void);    //���ƿ���ͳ��
    void RenderEventInfoToDlg(void);    //�����¼���Ϣ
	/**
	* @brief	ִ���ⲿ����
	*/
	void RenderExternal(void);

    void RenderAllCountData(bool isRetrack);
	/**
	* @brief	����ʵʱ�豸ϵͳʱ��
	*/
	void RenderDevTime(void);

	/**
	* @brief	���Ƹ����߼�����ID
	* @param[in]	bRefreshTrajectory���Ƿ�ˢ�¸�����
	*/

	void RenderTrajectory(bool bRefreshTrajectory);

	/**
	* @biref	��ո����߼�����ID
	*/
	void ClearTrajectory(void);

public:
    void RenderTrackCounter(bool bRefreshTrajectory);
protected:
	/**
	* @brief	�����㷨Ч��
	*/
	void RenderAlgoET(void);

	/**
	* @brief	��ȡ�ȴ���װ�İ�
	* @param[in]	nFrameNo����Ƶ֡���
	* @return	�ȴ���װ�İ�����
	*/
	CRenderObject* GetWaitPackage(unsigned int nFrameNo);

	/**
	* @brief	д¼��
	* @param[in]	lpByte��¼������
	* @param[in]	nLen����Ҫд��¼�����ݳ���
	*/
	void WriteRecord(LPBYTE lpByte, unsigned int nLen);

	bool					bInitialized;		// ����Ƿ�ִ���ѳ�ʼ��
	
	HWND					hWndDisplay;		// ��Ҫ���ƵĴ��ھ��
	unsigned int			wndWidth;			// ��Ҫ���ƵĴ��ھ��ʵ�ʿ��
	unsigned int			wndHeight;			// ��Ҫ���ƵĴ��ھ��ʵ�ʸ߶�
	
	HANDLE					hTimer;				// �ɵȴ���ʱ�����
	volatile DWORD			dwID;				// ����ģ��ID
	volatile HWND			hPCWnd;				// ��ʾ����ͳ�Ƶ����ݵĴ���
	bool					bIFrame;			// �Ѿ����յ�I֡

	CRenderObject*			nextRenderObject;	// ��һ֡����ʹ�õĶ���
	CRenderObject*			curRenderObject;	// ��ǰ����ʹ�õĶ���
	int						delayRound;			// ��ǰ��Ҫ���������ֵĻ���
	CConcurrentQueue<CRenderObject*>	renderingBufferQueue;	// ���ƻ������
	LPBYTE					renderBuffer;		// �����û�����
	unsigned int			bufferLen;			// �����û�������С
	unsigned int			dataLen;			// ���ݳ���
	unsigned int			nCount;				// �ۼ�û����Ƶ֡��Ҫ��ʾ������
	bool					clearBuffer;		// ��ʶ����ʾ����ʱ��ʱ����ջ���
	volatile bool			bStart;				// �Ƿ�ʼ������Ƶ֡����ʾ

	CRenderObject*			waitPackage;		// �ȴ����ݵ���İ�
//	std::map<unsigned int, CRenderObject*>	renderObjectMap;		// �ȴ�֡�����ݰ��ռ���ɵ�ɢ�б�

	CRenderObject*			renderObjectArray;	// ���ƶ���
	CConcurrentStack<CRenderObject*>	renderObjectStack;			// ������л��ƶ���
	std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>> trajectory;	// �����ߵĵ㼯��

    //std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>> trackCounter;	// �����ߵĵ㼯��

	// �����
	HINSTANCE				hIns;				// ���붯̬���ӿ�ľ��
	HANDLE					hDecoder;			// �������ľ��
	CRITICAL_SECTION		hCsDecoder;			// �������������ٽ���
	InitDecoderFunc			initDecoder;		// ��ʼ��������
	DecodeStreamFunc		decodeStream;		// ����������
	GetYUVStreamFunc		getYUVStream;		// ��ȡYUV����
	CloseDecoderFunc		closeDecoder;		// �رս�����

	CRITICAL_SECTION		hCs_component;				// ��������������ƹ��̵�ģ����ٽ���
	IRenderingComponent*	m_component;		// ������Ҫ������ƹ��̵�ģ��
	volatile bool			m_showPC;			// �Ƿ���ʾ��������
	volatile bool			m_showSysTime;		// �Ƿ���ʾ�豸ʱ��
	volatile bool			m_showTrajectory;	// �Ƿ���ʾ������
	volatile bool			m_showObjectID;		// �Ƿ���ʾ���ٶ���ID
	volatile bool			m_showAlgoET;		// �Ƿ���ʾ�㷨Ч��

    volatile bool           m_showDelayManCount; // �Ƿ���ʾ�������� Add by WangJie [2014-8-6]
    volatile bool           m_showDelayMaxTime;  // �Ƿ���ʾ�������ʱ�� Add by WangJie [2014-8-6]
    volatile bool           m_showDelayTotalTime;// �Ƿ���ʾ������ʱ�� Add by WangJie [2014-8-6]

	volatile bool			m_record;			// �Ƿ�¼��
	volatile bool			m_bRecordIFrame;	// ����¼����Ƿ��ҵ�I֡
	CAviWriter				aviFile;			// дAVI�ļ�ģ��
	TCHAR					m_strRecordPath[MAX_PATH];	// ¼���ļ�·��
	volatile int			m_nRecordedFrameCount;		// �Ѿ�¼���֡��		
	CRITICAL_SECTION		m_csRecordOp;		// ¼��ʹ�õ��ٽ���
	volatile unsigned int	m_nIgnoreFrameNo;	// ���Ե�֡���
	CGdiPlusDraw			m_gdiDraw;			// GDI+����ģ��

	// ����ģ��
	static long				nInstance;			// ʵ������
	static HANDLE			hRenderingThread;	// ���ƹ����̷߳��صľ��
	static bool				bExitThread;		// �˳��̵߳ı�ʶ
	static std::vector<CRenderingMod*>	vRenderingMod;	// ����ģ������
	//static CCriticalSection	csRenderingMod;		// ����ģ����
	static HWND				hContainer;			// �����Ĵ��ھ��
    
public:
	static CDraw*			drawMod;			// ����ģ��
	static bool				bIsDirectX;			// ��ʶ�Ƿ�ʹ��DirectX��ͼ

    char *renderData;
};

#endif