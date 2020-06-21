#pragma once
class GetLastErrorToString
{
public:
	operator CString();
	operator LPCTSTR();	///	<summery>Returns temporary address of message. DO NOT USE FOR LONG TIME, OR COPY DATA IMMEDIATELY.</summery>
	void CreateMsg();
private:
	CString m_msg;
	DWORD m_ID;
};