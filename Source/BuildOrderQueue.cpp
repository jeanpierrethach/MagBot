#include "BuildOrderQueue.h"

using namespace MagBot;

BuildOrderQueue::BuildOrderQueue()
	: _highest_priority(0)
	, _lowest_priority(0)
	, _default_priority_spacing(10)
	, num_skipped_items(0)
{
}


BuildOrderQueue::~BuildOrderQueue()
{
}

void BuildOrderQueue::clearAll()
{
	_queue.clear();
	_highest_priority = 0;
	_lowest_priority = 0;
}

BuildOrderItem & BuildOrderQueue::getHighestPriorityItem()
{
	num_skipped_items = 0;
	return _queue.front();
}

BuildOrderItem & BuildOrderQueue::getNextHighestPriorityItem()
{
	return _queue[num_skipped_items];
}

void BuildOrderQueue::queueAsHighestPriority(MetaType meta_type, bool blocking)
{
	int newPriority = _highest_priority + _default_priority_spacing;
	queueItem(BuildOrderItem(meta_type, newPriority, blocking));
}

void BuildOrderQueue::queueAsLowestPriority(MetaType meta_type, bool blocking)
{
	int newPriority = _lowest_priority - _default_priority_spacing;
	queueItem(BuildOrderItem(meta_type, newPriority, blocking));
}

void BuildOrderQueue::skipItem()
{
	++num_skipped_items;
}

bool BuildOrderQueue::canSkipItem()
{
	bool bigEnough = _queue.size() > (size_t)(1 + num_skipped_items);
	if (!bigEnough)
	{
		return false;
	}
	return (!_queue[num_skipped_items].blocking);
}

void BuildOrderQueue::removeHighestPriorityItem()
{
	// remove the back element of the vector
	_queue.pop_front();

	// if the list is not empty, set the highest accordingly
	_highest_priority = _queue.empty() ? 0 : _queue.front().priority;
	_lowest_priority = _queue.empty() ? 0 : _lowest_priority;
}

void BuildOrderQueue::removeCurrentHighestPriorityItem()
{
	_queue.erase(_queue.begin() + num_skipped_items);

	// if the list is not empty, set the highest accordingly
	_highest_priority = _queue.empty() ? 0 : _queue.front().priority;
	_lowest_priority = _queue.empty() ? 0 : _lowest_priority;
}

void BuildOrderQueue::queueItem(BuildOrderItem build_order_item)
{
	if (_queue.empty())
	{
		_highest_priority = build_order_item.priority;
		_lowest_priority = build_order_item.priority;
	}

	// push the item into the queue
	if (build_order_item.priority >= _highest_priority)
	{
		_queue.push_front(build_order_item);
	}
	else
	{
		_queue.push_back(build_order_item);
	}

	// if the item is somewhere in the middle, we have to sort again
	if ((_queue.size() > 1) && (build_order_item.priority < _highest_priority) && (build_order_item.priority > _lowest_priority))
	{
		// sort the list in ascending order, putting highest priority at the top
		std::sort(_queue.begin(), _queue.end());
	}

	// update the highest or lowest
	_highest_priority = (build_order_item.priority > _highest_priority) ? build_order_item.priority : _highest_priority;
	_lowest_priority = (build_order_item.priority < _lowest_priority) ? build_order_item.priority : _lowest_priority;
}

BuildOrderItem BuildOrderQueue::operator [] (int index)
{
	return _queue[index];
}

size_t BuildOrderQueue::size()
{
	return _queue.size();
}

bool BuildOrderQueue::isEmpty()
{
	return (_queue.size() == 0);
}