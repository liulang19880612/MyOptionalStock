#pragma once
#include "../GlobalUnits.h"

struct ItemData
{
	std::string m_strID;
};

class CAdapter_EmotionTileView : public SAdapterBase
{
public:
	struct IListener
	{
		virtual void OnEmotionItemClick(const std::string& strID) = 0;
	};
public:
	CAdapter_EmotionTileView(IListener* pListener)
		:m_pListener(pListener)
	{

	}
	~CAdapter_EmotionTileView(){}

protected:
	virtual int getCount(){
		return static_cast<int>(m_vecItems.size());
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (0 == pItem->GetChildrenCount())
			pItem->InitFromXml(xmlTemplate);

		size_t sPos = static_cast<size_t>(position);
		if (sPos >= m_vecItems.size()) return;

		ItemData* pItemData = m_vecItems[sPos];
		if (NULL == pItemData) return;

		SImageWnd* pImg = pItem->FindChildByName2<SImageWnd>(L"emotion");
		if (NULL != pImg)
		{
			pItem->GetEventSet()->
				subscribeEvent(EventItemPanelClick::EventID,
				Subscriber(&CAdapter_EmotionTileView::OnEvtItemPanelClick, this));

			std::map<std::string, IBitmap*>::iterator iter =
				CGlobalUnits::GetInstance()->m_mapFace.find(pItemData->m_strID);
			if (iter != CGlobalUnits::GetInstance()->m_mapFace.end())
			{
				pImg->SetImage(iter->second);
			}
		}
	}

	bool OnEvtItemPanelClick(EventArgs* e)
	{
		EventItemPanelClick* pItemClickEvt = sobj_cast<EventItemPanelClick>(e);
		if (NULL == pItemClickEvt)
			return false;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pItemClickEvt->sender);
		if (pItem)
		{
			int nItem = static_cast<int>(pItem->GetItemIndex());
			ItemData* pData = m_vecItems[nItem];
			if (NULL != pData)
			{
				m_pListener->OnEmotionItemClick(pData->m_strID);
				return true;
			}
		}
		return false;
	}

public:
	void AddItem(const std::string& strID)
	{
		ItemData* pItemFaceData = new ItemData;
		pItemFaceData->m_strID = strID;

		m_vecItems.push_back(pItemFaceData);
		notifyDataSetChanged();
	}

private:
	IListener*		m_pListener;
	std::vector<ItemData*>	m_vecItems;
};
class CEmojiDlg : public SHostWnd, public CAdapter_EmotionTileView::IListener
{
public:
	struct IListener
	{
		virtual void EmotionTileViewItemClick(const std::string& strID) = 0;
	};
public:
	CEmojiDlg(IListener* pListner);
	virtual ~CEmojiDlg();
protected:
	virtual void OnEmotionItemClick(const std::string& strID);
protected:
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnSize(UINT nType, SOUI::CSize size);
	void OnLButtonDown(UINT nFlags, SOUI::CPoint pt);
	void OnTimer(UINT_PTR idEvent);
	void OnSysCommand(UINT nID, SOUI::CPoint pt);
protected:
	EVENT_MAP_BEGIN()
	EVENT_MAP_END()

	BEGIN_MSG_MAP_EX(CEmojiDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	IListener* m_pListner;
};

