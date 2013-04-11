#ifndef __DUI_REF_H__
#define __DUI_REF_H__

#include "DUIDef.h"

DUI_BGN_NAMESPCE

//T's base class for CRefPtr<T>
class DUILIB_API CRefCountBase
{
public:
	CRefCountBase()
	{
		m_nRefCount = 0;
	}

	int GetRefCount() const
	{
		return m_nRefCount;
	}

	int AddRefCount()
	{
		return ++m_nRefCount;
	}

	int SubRefCount()
	{
		return --m_nRefCount;
	}

	VOID ResetRefCount()
	{
		m_nRefCount = 0;
	}


private:
	int	m_nRefCount;
};

template<typename T>
class DUILIB_API CRefPtr
{
public:
	T* operator->() const
	{
		return m_pRawObj;
	}

	T& operator()() const
	{
		return *m_pRawObj;
	}

	T& operator*() const
	{
		return *m_pRawObj;
	}

	T* GetPtr() const
	{
		return m_pRawObj;
	}

	bool IsNull() const
	{
		return m_pRawObj == NULL;
	}

	CRefPtr()
	{
		m_pRawObj = NULL;
	}

	CRefPtr(T* p)
	{
		m_pRawObj = p;
		if(p != NULL)
		{
			p->AddRefCount();
		}
	}

	CRefPtr(const CRefPtr& ref)
	{
		m_pRawObj = ref.m_pRawObj;
		if(m_pRawObj != NULL)
		{
			m_pRawObj->AddRefCount();
		}
	}

	~CRefPtr()
	{
		if(m_pRawObj != NULL && m_pRawObj->SubRefCount() == 0)
		{
			delete m_pRawObj;
		}
	}

	CRefPtr& operator = (const CRefPtr& ref)
	{
		if(this != &ref)
		{
			if(m_pRawObj != NULL
				&& m_pRawObj->SubRefCount() == 0)
			{
				delete m_pRawObj;
			}

			m_pRawObj = ref.m_pRawObj;

			if(m_pRawObj != NULL)
			{
				m_pRawObj->AddRefCount();
			}
		}

		return *this;
	}

	bool operator == (const CRefPtr& ref) const
	{
		return m_pRawObj == ref.m_pRawObj;
	}

	bool operator != (const CRefPtr& ref) const
	{
		return m_pRawObj != ref.m_pRawObj;
	}

private:
	T* m_pRawObj;
};

class DUILIB_API CNoCopy
{
public:
	CNoCopy() { NULL; }

private:
	CNoCopy(const CNoCopy& c);
	CNoCopy& operator = (const CNoCopy& c);
};

template <typename T> 
class DUILIB_API CSingletonT: public CNoCopy
{
public:
    static T* Instance()
    {
        if (m_pDUISingletonInstance == NULL) m_pDUISingletonInstance = new T;
        return m_pDUISingletonInstance;
    };
    static void DestroyInstance()
    {
		if ( m_pDUISingletonInstance != NULL)
			delete m_pDUISingletonInstance;
        m_pDUISingletonInstance = NULL;
    };

private:
    static T * m_pDUISingletonInstance; 
};

 template <typename T> T * CSingletonT<T>::m_pDUISingletonInstance = NULL;

DUI_END_NAMESPCE

#endif //__DUI_REF_H__