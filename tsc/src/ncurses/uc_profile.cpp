/**
 * uc_profile.cpp
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  zhangjiliang(张继良)
 * E-Mail:  pony_fx@hotmail.com
 * Date:    2006-08-09 14:16:59 中国标准时间
 * 
 * $log$
 *
**/
#include <sstream>
#include <fstream> 
#include "uc_profile.hpp"
uc_Profile::uc_Profile()
{
	m_bReadFile=0;
}
uc_Profile::~uc_Profile()
{
}
int uc_Profile::open(const std::string& iniFile)
{
	m_iniFile = iniFile;
	m_strText.erase(m_strText.begin(),m_strText.end());
	FILE* fp = fopen(iniFile.c_str(),"r");
	if(fp == NULL)
		return -1;
	fclose(fp);
	return 0;
}
int uc_Profile::create(const std::string& iniFile)
{
	m_iniFile = iniFile;
	FILE* f = fopen(iniFile.c_str(),"w");
	if(f == NULL)
		return -1;
	fclose(f);
	return 0;
}
int uc_Profile::close()
{
	return 0;
}
//找到节的开始位置
int uc_Profile::FindToSection(std::fstream &fp)
{
	std::string strTemp;
	char szTemp[200];
	std::string::size_type pos=0;
	while(!fp.eof())
	{
		fp.getline(szTemp,200);
		strTemp=szTemp;
		if(strTemp.empty())
			break;
		if(strTemp.at(0)!='[')
			continue;
		pos=strTemp.find(m_currentSection,0);			
		if(pos!=std::string::npos)//找到节的位置
			return 0;
	}
	return -1;
}
//找到到指定的字符串
int uc_Profile::FindToString(std::fstream &fp,const std::string &name,std::string &strValue)
{
	std::string strTemp;
	std::string::size_type pos=0;
	char szTemp[200];
	while(!fp.eof())
	{
		fp.getline(szTemp,200);
		strTemp=szTemp;
		if(strTemp.empty())
			break;
		if(strTemp.at(0)=='[')
			return -1;
		pos=strTemp.find(name,0);			
		if(pos!=std::string::npos)//找到节的位置
		{
			pos=strTemp.find("=",pos);
			if(pos!=std::string::npos)
			{
				std::string str;
				str.assign(strTemp.begin(),strTemp.begin()+pos);
				Trim(str);
				if(str.compare(name)==0)
				{
					strValue.assign(strTemp.begin()+pos+1,strTemp.end()-1);
					return 0;
				}
			}			
		}
	}	
	return -1;
}
void uc_Profile::Trim(std::string &strValue)
{
	if(strValue.empty()) return;
	int nSize=strValue.size();
	int nSpaceNum=0;
	int i=0;
	for(i=0;i<nSize;i++)
	{
		if(strValue.at(i)!=' ')
		{
			break;
		}
		nSpaceNum++;
	}
	strValue.erase(strValue.begin(),strValue.begin()+nSpaceNum);
	nSize=strValue.size();
	nSpaceNum=0;
	for(i=nSize-1;i>0;i--)
	{
		if(strValue.at(i)!=' ')
		{
			break;
		}
		nSpaceNum++;
	}
	strValue.erase(strValue.end()-nSpaceNum,strValue.end());	
}
int uc_Profile::readInt(const std::string& name, int& nValue)
{
	std::fstream fp(m_iniFile.c_str(),std::ios::in);
	if(FindToSection(fp)!=0)
	{
		nValue=0;
		return -1;
	}
	std::string strValue;
	if(FindToString(fp,name,strValue)!=0)
	{
		nValue=0;
		fp.close();
		return -1;
	}
	else
	{ 
		nValue=atoi(strValue.c_str());
		fp.close();
	}
	return 0;
}
int uc_Profile::readString(const std::string& name, std::string& value)
{
	std::fstream fp(m_iniFile.c_str(),std::ios::in);
	if(FindToSection(fp)!=0)
	{
		value="";
		return -1;
	}
	if(FindToString(fp,name,value)==0)
		Trim(value);
	else
	{
		value="";
		fp.close();
		return -1;
	}
	fp.close();
	return 0;
}
int uc_Profile::readFile(std::vector<std::string> &strText)
{
	std::fstream fp(m_iniFile.c_str(),std::ios::in);
	char szTemp[200];
	std::string strTemp;

	strText.clear();	
	while(!fp.eof())
	{
		fp.getline(szTemp,200);
		strTemp=szTemp;
		if(strTemp.empty())
			break;
		strTemp=strTemp+"\n";
		strText.push_back(strTemp);
	}
	fp.close();
	return 0;
}
int uc_Profile::writeFile(std::vector<std::string>&strText)
{
	std::fstream fp(m_iniFile.c_str(),std::ios::out);
	std::string strTemp;
	int nSize=strText.size();
	int i=0;
	for(i=0;i<nSize;i++)
	{
		strTemp=strText.at(i);
		fp.write(strTemp.c_str(),strTemp.size());
		fp.flush();
	}
	fp.close();
	return 0;
}
int uc_Profile::FindToSection(std::vector<std::string>&strText,int& nPlace)
{
	std::string::size_type pos=0;
	int nSize=strText.size();
	std::string strTemp;
	int i=0;
	for(i=0;i<nSize;i++)
	{
		strTemp=strText.at(i);
		if(strTemp.at(0)!='[')
			continue;
		pos=strTemp.find(m_currentSection,0);			
		if(pos!=std::string::npos)//找到节的位置
		{
			nPlace=i;
			return 0;		
		}
	}
	return -1;
}
int uc_Profile::FindToString(std::vector<std::string>&strText,const std::string &name,const std::string &strValue,int nPlace)
{
	std::string strTemp;
	std::string::size_type pos=0;
	int nSize=strText.size();
	int i=0;
	for(i=nPlace;i<nSize;i++)
	{
		strTemp=strText.at(i);
		if(strTemp.at(0)=='[')
		{
			std::string str;	
			str=name+"="+strValue+"\r\n";
			strText.insert(strText.begin()+i-1,str);			
			return 0;
		}
		pos=strTemp.find(name,0);			
		if(pos!=std::string::npos)//找到节的位置
		{
			pos=strTemp.find("=",pos);
			if(pos!=std::string::npos)
			{
				std::string str;
				str.assign(strTemp.begin(),strTemp.begin()+pos);
				Trim(str);
				if(str.compare(name)==0)
				{
					strText[i]=name+"="+strValue+"\r\n";
					return 0;
				}
			}
		}
	}
	std::string str;	
	str=name+"="+strValue+"\r\n";
	strText.insert(strText.begin()+i,str);
	return 0;
}
int uc_Profile::setAmendValue(std::vector<std::string>&strText,const std::string &name,const std::string& value)
{
	int nPlace=0;
	if(FindToSection(strText,nPlace)!=0)
	{
		std::string str;
		str=m_currentSection+"\r\n";
		strText.push_back(str);
		str=name+"="+value+"\r\n";
		strText.push_back(str);
		return 0;
	}
	FindToString(strText,name,value,nPlace+1);
	return 0;	
}
int uc_Profile::writeString(const std::string& name, const std::string& value)
{
	if(m_bReadFile==0)
	{
		m_bReadFile=1;
		readFile(m_strText);
	}
	setAmendValue(m_strText,name,value);
	writeFile(m_strText);
	return 0;			
}
int uc_Profile::writeInt(const std::string& name, int value)
{
	std::string strValue;
	char strTemp[128];
	sprintf(strTemp,"%d",value);
	strValue=strTemp;
	return writeString(name,strValue);
}
int uc_Profile::setSection(const std::string& section)
{
	m_currentSection ="["+section+"]";
	return 0;	
}
