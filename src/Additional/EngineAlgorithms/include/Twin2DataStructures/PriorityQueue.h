#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

namespace Twin2DataStrctures
{
    template<typename TElement, typename TPriority>
    class PriorityQueue {
    public:
        using Comparator = std::function<bool(const TPriority&, const TPriority&)>;

    private:
        std::vector<std::pair<TElement, TPriority>> _elements;
        Comparator comparator;

    public:
        using Comparator = std::function<bool(const TPriority&, const TPriority&)>;

        PriorityQueue(Comparator comparator) : comparator(comparator) {}

        size_t Count() const {
            return _elements.size();
        }

        void Enqueue(const TElement& element, const TPriority& priority) {
            auto it = std::lower_bound(_elements.begin(), _elements.end(), priority,
                [&](const std::pair<TElement, TPriority>& elem, const TPriority& value) {
                    return comparator(elem.second, value);
                });
            _elements.insert(it, std::make_pair(element, priority));
        }

        TElement Dequeue() {
            if (_elements.empty()) {
                return TElement();
            }
            auto element = _elements.front();
            _elements.erase(_elements.begin());
            return element.first;
        }
    };
}

#endif // !_PRIORITY_QUEUE_H_
