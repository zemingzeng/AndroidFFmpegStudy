//
// Created by mingzz on 2023/2/10.
//

/**
 * c++ queue :
 * 1.遵循先进先出
 * 2.方法size():queue元素数量，pop():移除第一个元素，
 *      front():访问第一个元素，push()从末尾添加一个元素，
 *      empty():queue里元素是否为空的
 * 3.queue没有迭代器，只能通过遍历并且移除的方式，访问每一个元素
 * c/c++多线程安全：
 * 1.pthread_cond_wait():线程进入wait状态后自动释放锁并阻塞，走出wait状态会自动获取锁，是原子操作
 * 2.pthread_cond_signal():唤醒因同一个cond阻塞的线程，有非原子操作,只唤醒一个
 * 3.pthread_mutex_lock():去竞争锁
 * 4.pthread_mutex_unlock():去释放锁
 */

#ifndef FFMPEG_AV_PACKET_QUEUE_H
#define FFMPEG_AV_PACKET_QUEUE_H


#include "queue"
#include "pthread.h"


extern "C"
{
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
}

class AVPacketQueue {

public:
    std::queue<AVPacket *> packetQueue;
    pthread_mutex_t mutex_t;
    pthread_cond_t cond_t;
private:
    bool working;

public:
    AVPacketQueue();

    ~AVPacketQueue();

    unsigned long size();

    void push(AVPacket *);

    void pull(AVPacket *);

    void freeAVPacket();

    void stopWorking() {
        working = false;
    }

    bool isWorking() {
        return working;
    }

};

#endif //FFMPEG_AV_PACKET_QUEUE_H
