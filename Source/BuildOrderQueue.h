#pragma once

#include "Common.h"

#include "MetaType.h"

namespace MagBot
{
	struct BuildOrderItem
	{
		MetaType meta_type;
		int priority;
		bool blocking;

		BuildOrderItem(MetaType m, int p, bool b)
			: meta_type(m)
			, priority(p)
			, blocking(b)
		{
		}

		bool operator < (const BuildOrderItem & x) const
		{
			return priority < x.priority;
		}
	};

	class BuildOrderQueue
	{
		// TODO design the queue
		/*
			What I need :

			a deque 
			overload the [] operator for the deque
			the deque will contain a struct of BuildOrderItem
			a buildOrderItem consist of a MetaType, priority

			the dequeue needs to be sorted after each insert
			int variables for lowest and highest priorities
			skippable items
			could have multiple items of same priority
		
		*/
		std::deque<BuildOrderItem> _queue;

		int _lowest_priority;
		int _highest_priority;
		int _default_priority_spacing;
		

	public:
		BuildOrderQueue();
		~BuildOrderQueue();
		
		int num_skipped_items;

		void clearAll();	
		void skipItem();
		bool canSkipItem();

		void queueAsHighestPriority(MetaType meta_type, bool blocking);
		void queueAsLowestPriority(MetaType meta_type, bool blocking);
		void queueItem(BuildOrderItem build_order_item);

		void removeHighestPriorityItem();
		void removeCurrentHighestPriorityItem();

		//int getHighestPriorityValue();								// returns the highest priority value
		//int	getLowestPriorityValue();								// returns the lowest priority value
		
		size_t size();													// returns the size of the queue
		bool isEmpty();

		//void removeAll(MetaType m);									// removes all matching meta types from queue

		//void drawQueueInformation(int x, int y);

		BuildOrderItem & getHighestPriorityItem();
		BuildOrderItem & getNextHighestPriorityItem();

		BuildOrderItem operator [] (int i);
	};

}