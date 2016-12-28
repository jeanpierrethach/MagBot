#pragma once

#include "Common.h"

#include "MetaType.h"

namespace MagBot
{
	struct BuildOrderItem
	{
		MetaType metaType;
		int priority;
		bool blocking;

		BuildOrderItem(MetaType m, int p, bool b)
			: metaType(m)
			, priority(p)
			, blocking(b)
		{
		}

		bool operator < (const BuildOrderItem &x) const
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

		int lowestPriority;
		int highestPriority;
		int defaultPrioritySpacing;
		//int numSkippedItems;

	public:
		BuildOrderQueue();
		~BuildOrderQueue();
		
		void clearAll();

		//void skipItem();											// increments skippedItems
		//void queueAsHighestPriority(MetaType m, bool blocking, bool gasSteal = false);		// queues something at the highest priority
		//void queueAsLowestPriority(MetaType m, bool blocking);		// queues something at the lowest priority
		
		void queueAsHighestPriority(MetaType m, bool blocking);
		void queueAsLowestPriority(MetaType m, bool blocking);
		void queueItem(BuildOrderItem b);
		void removeHighestPriorityItem();

		//void removeCurrentHighestPriorityItem();

		//int getHighestPriorityValue();								// returns the highest priority value
		//int	getLowestPriorityValue();								// returns the lowest priority value
		
		size_t size();													// returns the size of the queue

		bool isEmpty();

		//void removeAll(MetaType m);									// removes all matching meta types from queue

		//BuildOrderItem & getNextHighestPriorityItem();	// returns the highest priority item

		//bool canSkipItem();

		//void drawQueueInformation(int x, int y);


		BuildOrderItem & getHighestPriorityItem();

		BuildOrderItem operator [] (int i);
	};

}