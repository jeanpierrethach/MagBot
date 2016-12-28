#include "BuildOrderQueue.h"

using namespace MagBot;

BuildOrderQueue::BuildOrderQueue()
	: highestPriority(0)
	, lowestPriority(0)
	, defaultPrioritySpacing(10)
	//, numSkippedItems(0)
{
}


BuildOrderQueue::~BuildOrderQueue()
{
}

void BuildOrderQueue::clearAll()
{
	_queue.clear();

	highestPriority = 0;
	lowestPriority = 0;
}

BuildOrderItem & BuildOrderQueue::getHighestPriorityItem()
{
	// reset the number of skipped items to zero
	//numSkippedItems = 0;

	// the queue will be sorted with the highest priority at the back
	return _queue.back();
}

void BuildOrderQueue::queueAsHighestPriority(MetaType m, bool blocking)
{
	int newPriority = highestPriority + defaultPrioritySpacing;
	queueItem(BuildOrderItem(m, newPriority, blocking));
}

void BuildOrderQueue::queueAsLowestPriority(MetaType m, bool blocking)
{

	int newPriority = lowestPriority + defaultPrioritySpacing;
	queueItem(BuildOrderItem(m, newPriority, blocking));
}


void BuildOrderQueue::removeHighestPriorityItem()
{
	// remove the back element of the vector
	_queue.pop_back();

	// if the list is not empty, set the highest accordingly
	highestPriority = _queue.empty() ? 0 : _queue.back().priority;
	lowestPriority = _queue.empty() ? 0 : lowestPriority;
}

void BuildOrderQueue::queueItem(BuildOrderItem b)
{
	if (_queue.empty())
	{
		highestPriority = b.priority;
		lowestPriority = b.priority;
	}

	// push the item into the queue
	if (b.priority <= lowestPriority)
	{
		_queue.push_front(b);
	}
	else
	{
		_queue.push_back(b);
	}

	// if the item is somewhere in the middle, we have to sort again
	if ((_queue.size() > 1) && (b.priority < highestPriority) && (b.priority > lowestPriority))
	{
		// sort the list in ascending order, putting highest priority at the top
		std::sort(_queue.begin(), _queue.end());
	}

	// update the highest or lowest if it is beaten
	highestPriority = (b.priority > highestPriority) ? b.priority : highestPriority;
	lowestPriority = (b.priority < lowestPriority) ? b.priority : lowestPriority;
}

BuildOrderItem BuildOrderQueue::operator [] (int i)
{
	return _queue[i];
}

size_t BuildOrderQueue::size()
{
	return _queue.size();
}

bool BuildOrderQueue::isEmpty()
{
	return (_queue.size() == 0);
}