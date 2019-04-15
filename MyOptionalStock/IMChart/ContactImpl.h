#pragma  once 
//////////////////////////////////////////////////////////////////////////
//  ��ϵ�˹���
//////////////////////////////////////////////////////////////////////////

#include "../PageImplBase.h"

struct ContactItemData
{
	ContactItemData() :bGroup(false){}

	SStringT strImg;	// �û�ͼ��
	SStringT strName;	// �û���
	SStringT strID;     // �û�ID
	bool bGroup;        // �Ƿ���Ⱥ
	INT32 gid;			// �û�����ID
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