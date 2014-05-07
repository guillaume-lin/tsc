// uc_CarFluxInfo.cpp: implementation of the uc_CarFluxInfo class.
//
//////////////////////////////////////////////////////////////////////
#include <sstream>
#include <iomanip>
#include "uc_ErrorInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

uc_ErrorInfo::uc_ErrorInfo()
{

}

uc_ErrorInfo::~uc_ErrorInfo()
{

}
int uc_ErrorInfo::OpenWatchInfo(std::string &strProfile)
{
	if(m_Profile.open(strProfile)!=0)
	{
		if(m_Profile.create(strProfile)!=0)
		{
			return -1;
		}
		if(m_Profile.open(strProfile)!=0)
		{
			return -1;
		}
	}
	return 0;
}
void uc_ErrorInfo::SetPath(std::string& strPath)
{
	m_strWorkPath=strPath;
	if(m_strWorkPath[m_strWorkPath.size()-1] != '/')
		m_strWorkPath.append(1, '/');
	m_strIniFileName=m_strWorkPath+"ErrorInfo.ini";
}
void uc_ErrorInfo::Write(machine_status_t *pMachineStatus)
{
	//得到当前的条数
	int nRecordCount=0;
	m_Profile.setSection("Info");
	m_Profile.readInt( "RecordCount",nRecordCount);
	nRecordCount=nRecordCount+1;
	//写记录
	std::string strKey;
	std::string strValue;
	std::string strSection;
	std::ostringstream os;
	
	os.str("");
	os<<nRecordCount;
	strSection=os.str();
	m_Profile.setSection(strSection);
	//时间	
	char strTime[128];
	sprintf(strTime,"%02d%02d-%02d-%02d %02d:%02d:%02d",
		   pMachineStatus->m_year[0],pMachineStatus->m_year[1],
	          pMachineStatus->m_month,pMachineStatus->m_day,
	         pMachineStatus->m_hour,pMachineStatus->m_minute,
	         pMachineStatus->m_second);
	strValue=strTime;
	m_Profile.writeString("Time",strValue);
	//错误代码
	char strError[32];
	sprintf(strError,"%02x%02x",
		    pMachineStatus->m_error.m_hard_error,
		    pMachineStatus->m_error.m_soft_error);
	strValue=strError;	
	m_Profile.writeString("ErrorCode",strValue);
	//灯的状态
	lamp_health_t* h;
	h = &pMachineStatus->m_lamp_health;
	os.str("");
	int i=0;
	for(i=0; i<MAX_TERMINAL_COUNT ; i++){
		int hv = lamp_health_get_health(h,i+1);
		if(hv==1)//有故障
		    os<<i<<",";
	}
	strValue=os.str();
	m_Profile.writeString("LampState",strValue);
	//线圈的状态
	detector_health_t *pDetector_health;
	pDetector_health= &pMachineStatus->m_detector_health;
	os.str("");
	for(i=0; i<MAX_DETECTOR_COUNT; i++){
		int hv = lamp_health_get_health(h,i+1);
		if(hv==1)//有故障
		    os<<i<<",";
	}
	strValue=os.str();
	m_Profile.writeString("DetectorState",strValue);
	m_Profile.setSection("Info");
	m_Profile.writeInt("RecordCount",nRecordCount);
}
int uc_ErrorInfo::Update(machine_status_t *pMachineStatus)
{
	char strError[32];
	sprintf(strError,"%02x%02x",
		    pMachineStatus->m_error.m_hard_error,
		    pMachineStatus->m_error.m_soft_error);
	if(m_strError.compare(strError)==0)
		return 0;
	m_strError=strError;
	if(OpenWatchInfo(m_strIniFileName)==0)
	{
		Write(pMachineStatus);
		m_Profile.close();
		return 0;
	}
	return -1;
}
void uc_ErrorInfo::Load()
{
	if(OpenWatchInfo(m_strIniFileName)==0)
	{
		int nRecordCount=0;
		m_Profile.setSection("Info");
		m_Profile.readInt( "RecordCount",nRecordCount);
		//写记录
		std::string strSection;
		std::ostringstream os;
		
		os.str("");
		os<<nRecordCount;
		strSection=os.str();
		m_Profile.setSection(strSection);
		m_Profile.readString("ErrorCode",m_strError);
		m_Profile.close();
	}
}
