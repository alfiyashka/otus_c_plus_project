#ifndef _ConcurrentQueue_HPP_
#define _ConcurrentQueue_HPP_

#include <future>
#include <memory>
#include <mutex>
#include <utility>

template <typename T>
class ConcurrentQueue
{
private:
    struct Node
    {
        T data;
        std::unique_ptr<Node> next;
        Node(T data_): data(std::move(data)) {}
    };
    std::unique_ptr<Node> head;
    Node* tail;
    std::mutex headMutex;
    std::mutex tailMutex;
public:
    ConcurrentQueue(): head(new Node(T{}), tail(head.get())) {}
    ~ConcurrentQueue() = default;

    std::unique_ptr<T> pop()
    {
        std::lock_guard<std::mutex> headLock(headMutex);
        {
            std::lock_guard<std::mutex> tailLock(tailMutex);
            if (head.get() == tail)
            {
                throw std::out_of_range("The queue is empty!");
            }

            std::unique_ptr<T> res = std::make_unique<T>(std::move(head->data));
            std::unique_ptr<Node> oldHead = std::move(head);
            head = std::move(oldHead->next);
            return res;
        }
    }

    bool isEmpty() const
    {
        std::lock_guard<std::mutex> headLock(headMutex);
        {
            std::lock_guard<std::mutex> tailLock(tailMutex);
            return (head.get() == tail);
        }
    }

    void push(T val)
    {
        std::unique_ptr<Node> dummyNode = std::make_unique<Node>(Node(T{}));
        Node* newTail = dummyNode.get();
        std::lock_guard<std::mutex> tailLock(tailMutex);
        tail->next = std::move(dummyNode);
        tail->data = val;
        tail = newTail;
    }

    ConcurrentQueue(const ConcurrentQueue& other) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue& other) = delete;
    ConcurrentQueue(const ConcurrentQueue&& other) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&& other) = delete;

};



#endif