#pragma once

class CMessage
{
public:
	CMessage(void);
	~CMessage(void);

public:
	bool SetMessage(const MessageMap Type, void* pData);
	void* GetMessageData(MessageMap& Type);
	void SetCommand(MessageMap messageMap) {m_Type = messageMap;};
	MessageMap GetCommand() {return m_Type;};

	CMessage& operator = (const CMessage& newObj);

private:
	void ReleasMemer();

private:
	MessageMap m_Type;

	void* m_pVoid;
	int m_nBufLen;
};
