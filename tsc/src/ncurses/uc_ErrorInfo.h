// uc_CarFluxInfo.h: interface for the uc_CarFluxInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UC_CARFLUXINFO_H__B1CB880C_CC55_46E1_BE9A_41F984D9C2AA__INCLUDED_)
#define AFX_UC_CARFLUXINFO_H__B1CB880C_CC55_46E1_BE9A_41F984D9C2AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "uc_profile.hpp"
#include "../control/datastruct.h"
class uc_ErrorInfo  
{
public:
	uc_ErrorInfo();
	virtual ~uc_ErrorInfo();
public:
	void SetPath(std::string& strPath);	
	void Load();
	int  Update(machine_status_t *pMachineStatus);	
private:
	int OpenWatchInfo(std::string &strProfile);
	void Write(machine_status_t *pMachineStatus);
private:
	uc_Profile m_Profile;
	std::string m_strWorkPath;
	std::string m_strIniFileName;
	std::string m_strError;

};

#endif // !defined(AFX_UC_CARFLUXINFO_H__B1CB880C_CC55_46E1_BE9A_41F984D9C2AA__INCLUDED_)
