//
// Created by  mingzz on 2023/2/10.
//

#include "av_packet_queue.h"

AVPacketQueue::AVPacketQueue() : working(true) {
    pthread_cond_init(&cond_t, nullptr);
    pthread_mutex_init(&mutex_t, nullptr);
}

unsigned long AVPacketQueue::size() {
    pthread_mutex_lock(&mutex_t);
    unsigned long size = packetQueue.size();
    pthread_mutex_unlock(&mutex_t);
    return size;
}

AVPacketQueue::~AVPacketQueue() {
    freeAVPacket();
}

void AVPacketQueue::pull(AVPacket *outPacket) {

    pthread_mutex_lock(&mutex_t);

    while (true) {
        if (packetQueue.size() > 0) {
            AVPacket *avPacket = packetQueue.front();
            //copy
            int ret = av_packet_ref(outPacket, avPacket);
            if (ret == 0) {
                packetQueue.pop();
            }
            av_packet_free(&avPacket);
            //av_free(avPacket);
            avPacket = nullptr;
            break;
        } else {
            pthread_cond_wait(&cond_t, &mutex_t);
        }
    }
    pthread_mutex_unlock(&mutex_t);

}

void AVPacketQueue::push(AVPacket *inputPacket) {
    pthread_mutex_lock(&mutex_t);
    packetQueue.push(inputPacket);
    pthread_cond_signal(&cond_t);
    pthread_mutex_unlock(&mutex_t);
}

void AVPacketQueue::freeAVPacket() {
    //pthread_cond_signal(&cond_t);
    pthread_mutex_lock(&mutex_t);
    while (!packetQueue.empty()) {
        AVPacket *avPacket = packetQueue.front();
        packetQueue.pop();
        av_packet_free(&avPacket);
        //av_free(avPacket);
        avPacket = nullptr;
    }
    pthread_cond_destroy(&cond_t);
    pthread_mutex_destroy(&mutex_t);
    pthread_mutex_unlock(&mutex_t);
}

