#pragma once

//CSeriesData
//一个序列数据的模板类
//该模板类的目的是避免越界，如果越界，返回dataEmpty元素
//实现了读写数据的线程互斥
//由于不知道每个元素具体是什么东西，就把这个东西定义为模板T
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
	//为什么不返回const& T
	//因为返回const& T，以后将直接操作共享内存，而此时另一个线程可能会更新内存数据
	T& operator[](unsigned int x) 
	{
		return GetAt(x);
	}

	//输入参数x并不是元素的绝对下标，因为序列的第一个元素下标可能不是0
	//比如：某指标值不是计算全部K线，那么他的第一个元素就不是0
	
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
		//判断是否需要重新计算当前屏幕的最大最小值
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
		
		//判断是否需要重新计算当前屏幕的最大最小值
		m_bNeedRecalu = bRefresh;
	}

	int GetCount()
	{
		if (!m_bCopyData)
		{
			return m_nCount;
		}
		return m_arr.size();
	}		//返回数据个数
	int GetPrecision(){return m_nPrect;}
	
	int GetMaxIndex() //返回最大索引坐标
	{
		if (!m_bCopyData)
		{
			return m_nCount-1;
		}
		return m_arr.size()-1;
	}	

	void RecaluMinMax(){m_bNeedRecalu = TRUE;}
	BOOL NeedReCaluMinMax(){return m_bNeedRecalu;}		//是否需要从新计算最大最小值
	
	vector<T> m_arr;
	
	void SetCopyData(bool bCopy)
	{
		m_bCopyData = bCopy;
	}
	
private:
	bool m_bCopyData;
	T m_dataEmpty;		//用于访问越界时，返回无效数据
	int m_nPrect;	//精确度
	BOOL m_bNeedRecalu;
	T* m_pBuffer;
	ULONG m_nCount;
};
