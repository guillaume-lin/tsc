/**
 * uc_profile.hpp
 *
 * use WritePrivateProfileSection .. . etc function to store ini information
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  zhangjiliang(张继良)
 * E-Mail:   pony_fx@hotmail.com
 * Date:    2006-08-10 21:11:50 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_UC_PROFILE_HPP
#define INCLUDED_UC_PROFILE_HPP
#include <string>
#include <vector>

class uc_Profile {
	public:
		uc_Profile();
		~uc_Profile();
	public:
		int open(const std::string& iniFile);
		int create(const std::string& iniFile);

		int close();
		int setSection(const std::string& section);
		int readInt(const std::string& name, int & nValue);
		int readString(const std::string& name, std::string& value);
		int writeInt(const std::string& name, int value);
		int writeString(const std::string& name, const std::string& value);
	private:
		int  readFile(std::vector<std::string> &strText);
		int  writeFile(std::vector<std::string>&strText);
		int  setAmendValue(std::vector<std::string>&strText,const std::string &name,const std::string& value);
		void Trim(std::string &strValue);
		int  FindToSection(std::fstream &fp);//找到指定的节
		int  FindToSection(std::vector<std::string>&strText,int &nPlace);
		int  FindToString(std::fstream &fp,const std::string &name,std::string &strValue);//找到指定位置
		int  FindToString(std::vector<std::string>&strText,const std::string &name,const std::string &strValue,int nPlace);
	private:
		std::string m_iniFile;
		std::string m_currentSection;
		std::vector<std::string> m_strText;
		int  m_bReadFile;
};
#endif

