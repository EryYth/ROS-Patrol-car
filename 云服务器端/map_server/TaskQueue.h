#pragma once
#include <queue>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;

// ��������ṹ��
using callback = void(*)(void*);

struct Task
{
    Task()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task(callback f, void* arg)
    {
        function = f;
        this->arg = arg;
    }
    callback function;
    void* arg;
};

// �������
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();

    // �������
    void addTask(Task& task);
    void addTask(callback func, void* arg);

    // ȡ��һ������
    Task takeTask();

    // ��ȡ��ǰ�������������
    inline int taskNumber()
    {
        return (int)m_queue.size();
    }

private:
    pthread_mutex_t m_mutex;    // ������
    std::queue<Task> m_queue;   // �������
};
