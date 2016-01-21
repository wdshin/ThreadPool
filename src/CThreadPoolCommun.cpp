#include"../header/CThreadPoolCommun.h"
#include<utility>	//move
#include"../../lib/header/thread/CThreadList.h"
#include"../../lib/header/thread/CThreadQueue.h"
#include"../header/CThreadPoolItem.h"
using namespace std;

namespace nThread
{
	void CThreadPoolCommunBase::detach_(){}
	
	void CThreadPoolCommunBase::finish_(){}
	
	void CThreadPoolCommunBase::join_(){}

	void CThreadPoolCommunBase::communPoolDetach()
	{
		detach_();
	}

	void CThreadPoolCommunBase::communPoolFinish()
	{
		finish_();
	}

	void CThreadPoolCommunBase::communPoolJoin()
	{
		join_();
	}

	CThreadPoolCommunBase::~CThreadPoolCommunBase(){}

	struct CThreadPoolCommun::Impl
	{
		CThreadPoolItem *item;
		CThreadList<CThreadPoolItem*> *join_anyList;	//or use deque
		CThreadQueue<CThreadPoolItem*> *waitingQue;
		Impl(CThreadPoolItem *,CThreadList<CThreadPoolItem*> *,CThreadQueue<CThreadPoolItem*> *);
		void destroy();
		inline void detach()
		{
			waitingQue->emplace(item);
		}
		inline void func_is_completed()	//notify CThreadPool::join_any
		{
			join_anyList->emplace_back(item);
		}
	};

	CThreadPoolCommun::Impl::Impl(CThreadPoolItem *item_,CThreadList<CThreadPoolItem*> *join_anyList_,CThreadQueue<CThreadPoolItem*> *waitingQue_)
		:item{item_},join_anyList{join_anyList_},waitingQue{waitingQue_}{}

	void CThreadPoolCommun::Impl::destroy()
	{
		join_anyList->remove_if([&](const CThreadPoolItem *val){return val->get_id()==item->get_id();});
		//if CThreadPool::join_any run first, this would not erase anything (it's ok)
		waitingQue->emplace(item);
	}

	CThreadPoolCommun::CThreadPoolCommun(CThreadPoolItem *item,CThreadList<CThreadPoolItem*> *join_anyList,CThreadQueue<CThreadPoolItem*> *waitingQue)
		:impl_{item,join_anyList,waitingQue}{}

	CThreadPoolCommun::CThreadPoolCommun(CThreadPoolCommun &&rVal) noexcept
		:impl_{move(rVal.impl_)}{}

	void CThreadPoolCommun::destroy()
	{
		impl_.get().destroy();
	}

	void CThreadPoolCommun::detach()
	{
		impl_.get().detach();
	}

	void CThreadPoolCommun::func_is_completed()
	{
		impl_.get().func_is_completed();
	}

	CThreadPoolCommun& CThreadPoolCommun::operator=(CThreadPoolCommun &&rVal) noexcept
	{
		impl_=move(rVal.impl_);
		return *this;
	}

	CThreadPoolCommun::~CThreadPoolCommun(){}
}