#ifndef _IPVA_DTC_INETTRANSMISSION_H_
#define _IPVA_DTC_INETTRANSMISSION_H_

class INetTransmission
{
public:
	/**
	* @brief	向服务端发出请求
	* @param[in]	lpReq，指向请求内容的指针
	* @param[in]	nReqLen，请求内容的数据长度
	* @param[out]	lpResp，返回请求回复的缓冲区指针
	* @param[out]	nRespLen，返回请求回复的数据长度
	* @param[out]	nCode，错误码
	* @return	返回值，错误码的含义依靠返回值来确定
	*/
	virtual int ExecCmd(LPBYTE lpReq, unsigned int nReqLen, LPBYTE& lpResp, unsigned int& nRespLen, int& nCode) = 0;

	/**
	* @brief	注册控制权限
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	virtual int UpdatePrivileges(int& nCode) = 0;

	/**
	* @brief	注销控制权限
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	virtual int CancelPrivileges(int& nCode) = 0;
};

#endif
