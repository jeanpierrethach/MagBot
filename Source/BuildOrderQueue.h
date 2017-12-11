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

		// TODO has item on queue (contains)
		/*const bool BuildOrderItem::operator == (const BuildOrderItem & b)
		{
			return (b.priority == priority);
		}*/
	};

	class BuildOrderQueue
	{
		std::deque<BuildOrderItem> _queue;

		int _lowest_priority;
		int _highest_priority;
		int _default_priority_spacing;
		int _num_skipped_items;

	public:
		BuildOrderQueue();
		~BuildOrderQueue();

		void clearAll();	
		void skipItem();
		bool canSkipItem();

		void queueAsHighestPriority(MetaType meta_type, bool blocking);
		void queueAsLowestPriority(MetaType meta_type, bool blocking);
		void queueItem(BuildOrderItem build_order_item);

		void removeHighestPriorityItem();
		void removeCurrentHighestPriorityItem();
		
		const size_t size() const;
		bool isEmpty();

		int getSkippedItemsCount() const;

		bool hasItemInQueue(BWAPI::UnitType unit_type);

		BuildOrderItem & getHighestPriorityItem();
		BuildOrderItem & getNextHighestPriorityItem();

		BuildOrderItem operator [] (int i);
	};

}