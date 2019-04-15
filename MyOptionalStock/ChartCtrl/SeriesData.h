#pragma once

//CSeriesData
//һ���������ݵ�ģ����
//��ģ�����Ŀ���Ǳ���Խ�磬���Խ�磬����dataEmptyԪ��
//ʵ���˶�д���ݵ��̻߳���
//���ڲ�֪��ÿ��Ԫ�ؾ�����ʲô�������Ͱ������������Ϊģ��T
//

#include <vector>

using namespace std;

template <typename T>
class CSeriesData
{
public:
	CSeriesData()
	{
		memset(&m_dataEmpty,0,sizeof(T));
		m_bCopyData = true;
		m_pBuffer = NULL;
		m_nCount=0;
	}

	virtual ~CSeriesData()
	{
		
	}

	bool IsEmpty()
	{
		if (!m_bCopyData)
		{
			return m_nCount == 0;
		}
		return m_arr.empty();
	}
	//Ϊʲô������const& T
	//��Ϊ����const& T���Ժ�ֱ�Ӳ��������ڴ棬����ʱ��һ���߳̿��ܻ�����ڴ�����
	T& operator[](unsigned int x) 
	{
		return GetAt(x);
	}

	//�������x������Ԫ�صľ����±꣬��Ϊ���еĵ�һ��Ԫ���±���ܲ���0
	//���磺ĳָ��ֵ���Ǽ���ȫ��K�ߣ���ô���ĵ�һ��Ԫ�ؾͲ���0
	
	T& GetAt(unsigned int idx)    
	{
		if (!m_bCopyData)
		{
			if (m_nCount == 0 || idx<0 || idx>m_nCount - 1)
				return m_dataEmpty;
			else
				return m_pBuffer[idx];
		}

		if( m_arr.empty() || idx<0) return m_dataEmpty;
		
		if(idx>=m_arr.size()) 
			idx=m_arr.size()-1;
	
		return m_arr[idx]; 
	}
	void AddData(T* pData, int nCount, int nPrect, BOOL bRefresh)
	{
		if (nCount<1 || pData==NULL) return;
		m_nPrect = nPrect;
		m_arr.resize(nCount + m_arr.size());
		memcpy(m_arr.data() + oldSize, pData, sizeof(T)*nCount);
		//�ж��Ƿ���Ҫ���¼��㵱ǰ��Ļ�������Сֵ
		m_bNeedRecalu = bRefresh;
	}
	void SetData(T* pData, int nCount, int nPrect,BOOL bRefresh)
	{
		if (!m_bCopyData)
		{
			m_pBuffer = pData;
			m_nCount = nCount;
			m_nPrect = nPrect;
			m_bNeedRecalu = bRefresh;
		}
		if(nCount<1) return;
		
		m_arr.clear();
		

		if((int)m_arr.size()>nCount) return;

		m_nPrect = nPrect;
		
		

		auto nArrCount = m_arr.size();
		
		if(nArrCount>0)
		{
			m_arr[nArrCount-1] = pData[nArrCount-1];
		}
		
		if(nCount>(int)nArrCount)
		{
			m_arr.resize(nCount);
			memcpy(m_arr.data(), pData, sizeof(T)*nCount);

		}
		
		//�ж��Ƿ���Ҫ���¼��㵱ǰ��Ļ�������Сֵ
		m_bNeedRecalu = bRefresh;
	}

	int GetCount()
	{
		if (!m_bCopyData)
		{
			return m_nCount;
		}
		return m_arr.size();
	}		//�������ݸ���
	int GetPrecision(){return m_nPrect;}
	
	int GetMaxIndex() //���������������
	{
		if (!m_bCopyData)
		{
			return m_nCount-1;
		}
		return m_arr.size()-1;
	}	

	void RecaluMinMax(){m_bNeedRecalu = TRUE;}
	BOOL NeedReCaluMinMax(){return m_bNeedRecalu;}		//�Ƿ���Ҫ���¼��������Сֵ
	
	vector<T> m_arr;
	
	void SetCopyData(bool bCopy)
	{
		m_bCopyData = bCopy;
	}
	
private:
	bool m_bCopyData;
	T m_dataEmpty;		//���ڷ���Խ��ʱ��������Ч����
	int m_nPrect;	//��ȷ��
	BOOL m_bNeedRecalu;
	T* m_pBuffer;
	ULONG m_nCount;
};
