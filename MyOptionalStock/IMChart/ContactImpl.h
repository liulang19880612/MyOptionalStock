#pragma  once 
//////////////////////////////////////////////////////////////////////////
//  联系人管理
//////////////////////////////////////////////////////////////////////////

#include "../PageImplBase.h"

struct ContactItemData
{
	ContactItemData() :bGroup(false){}

	SStringT strImg;	// 用户图像
	SStringT strName;	// 用户名
	SStringT strID;     // 用户ID
	bool bGroup;        // 是否是群
	INT32 gid;			// 用户的组ID
};
struct IContactListener
{
	virtual void ContactItemClick(int nGID, const std::string& strID) = 0;
	virtual void ContactItemDBClick(int nGID, const std::string& strID) = 0;
	virtual void ContactItemRClick(int nGID, const std::string& strID) = 0;
};
class CContactImpl : public CPageImplBase, public  STreeAdapterBase<ContactItemData>
{
public:
	CContactImpl(IContactListener*pListener);
	~CContactImpl();
public:
	virtual  void OnInit(SWindow*pRoot);
	virtual  SStringW GetPageName(){ return L"contact_page"; }
	virtual void getView(SOUI::HTREEITEM loc, SWindow * pItem, pugi::xml_node xmlTemplate);
	virtual int getViewType(SOUI::HTREEITEM hItem) const
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (ii.data.bGroup) return 0;
		else return 1;
	}	
	virtual int getViewTypeCount() const
	{
		return 2;
	}
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pPageRoot)
		CHAIN_EVENT_MAP(CPageImplBase)
	EVENT_MAP_END()
	IContactListener* m_pContactListener;
};