#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "greenwireless_m.h"
//#include <Windows.h>
#include <queue>
#include <mutex>
#include <random>
#include <chrono>
using namespace std;
using namespace omnetpp;



class Edge : public cSimpleModule
{
  private:
    greenMsg *outMsg = nullptr;
    cMessage * timer;
    queue <greenMsg*> first;
    queue <greenMsg*> second;
    queue <greenMsg*> third;
    int first_queue_size = 310;
    int second_queue_size = 10;
    int third_queue_size = 10;
    int time = 0;
    int duration = 1;  //seconds
    int energyForEachPacket = 1; // energy that should be used to send each packet.
    int maxPower = 800;   // max power
    int windEnergy = 100;
    int solarEnergy = 0;
    int bufferDropP1 = 0;
    int bufferDropP2 = 0;
    int bufferDropP3 = 0;
    std::mt19937 rng;
    mutex g_mutex;
    cHistogram greenStats;
    cOutVector greenEnergyVector;
    cOutVector totalEnergyVector;
    cOutVector dirtyEnergyVector;
  protected:
    virtual greenMsg *generateMessage(int priority);
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual double normal(double x, double miu, double sigma);
};

Define_Module(Edge);

void Edge::initialize()
{
    rng.seed(std::random_device()());
    greenEnergyVector.setName("green");
    totalEnergyVector.setName("total");
    dirtyEnergyVector.setName("dirty");
    if (par("alive").boolValue() == true){
        timer = new cMessage("one second timer");
        scheduleAt(0.0, timer);
    }
//    WATCH(&first);
}
double Edge::normal(double x, double miu, double sigma) {
    double PI = 3.14;
    return 1.0/(sqrt(2*PI)*sigma) * exp(-1*(x-miu)*(x-miu)/(2*sigma*sigma));
}
void Edge::handleMessage(cMessage *msg)
{

    if (time == 100){
        EV<<"Packet Drop: Priority 1, "<<" Number = "<<bufferDropP1<<"\n";
        EV<<"Packet Drop: Priority 2, "<<" Number = "<<bufferDropP2<<"\n";
        EV<<"Packet Drop: Priority 3, "<<" Number = "<<bufferDropP3<<"\n";
        return;
    }
    if (msg == timer){
        time++;
        scheduleAt(simTime() + duration, timer);
        // Generate 300 Priority 1
        int P1 = 300;
        for(int i = 0; i < P1; i++){
            if (first.size() < first_queue_size){
                outMsg = generateMessage(1);
                first.push(outMsg);
            }else{
                bufferDropP1 += 1;
                break;
            }
        }
        // Generate 10-50 Priority 2
        std::uniform_int_distribution<int> u10_50 (10, 50);
        int P2 = u10_50(rng);
        for(int i = 0; i < P2; i++){
            if (second.size() < second_queue_size){
                outMsg = generateMessage(2);
                second.push(outMsg);
            }
            else if (first.size() < first_queue_size){
                second.pop();
                outMsg = generateMessage(1);
                first.push(outMsg);
                outMsg = generateMessage(2);
                second.push(outMsg);
            }
            else{
                bufferDropP2 += 1;
                break;
            }
        }
        // Generate 0-20 Priority 3
        std::uniform_int_distribution<int> u0_20(0, 20);
        int P3 = u0_20(rng);
        for(int i = 0; i < P3; i++){
            if (third.size() < third_queue_size){
                outMsg = generateMessage(3);
                third.push(outMsg);
            }
            else if (second.size() < second_queue_size){
                third.pop();
                outMsg = generateMessage(2);
                second.push(outMsg);
                outMsg = generateMessage(3);
                second.push(outMsg);
            }
            else{
                bufferDropP3 += 1;
                break;
            }
        }

        std::uniform_int_distribution<int> u0_500 (0, 500);
        solarEnergy = u0_500(rng);
        int currentGreenEnergy = windEnergy + solarEnergy;
        int netEnergyUsed = 0;
        int dirtyEnergy = 0;
        greenStats.collect(currentGreenEnergy);
        greenEnergyVector.record(currentGreenEnergy);
        int restEnergy = maxPower - currentGreenEnergy;
        while(currentGreenEnergy >= 0){
            if(!first.empty()){
                if(currentGreenEnergy - energyForEachPacket >= 0){
                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
                    greenMsg *tmp = first.front();
                    first.pop();
                    forwardMessage(tmp);
                }else{
                    break;
                }
            }else if (!second.empty()){
                if(currentGreenEnergy - energyForEachPacket >= 0){
                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
                    greenMsg *tmp = second.front();
                    second.pop();
                    forwardMessage(tmp);
                }else{
                    break;
                }
            }else if (!third.empty()){
                if(currentGreenEnergy - energyForEachPacket >= 0){
                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
                    greenMsg *tmp = third.front();
                    third.pop();
                    forwardMessage(tmp);
                }else{
                    break;
                }
            }else{
                break;
            }
        }
        while(restEnergy > 0){
            if(!first.empty()){
                if(restEnergy - energyForEachPacket > 0){
                    restEnergy = restEnergy - energyForEachPacket;
                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
                    dirtyEnergy = dirtyEnergy + energyForEachPacket;
                    greenMsg *tmp = first.front();
                    first.pop();
                    forwardMessage(tmp);
                }else{
                    break;
                }
            }else{
                break;
            }
        }

        dirtyEnergyVector.record(dirtyEnergy);
        totalEnergyVector.record(netEnergyUsed);
    }
}

greenMsg *Edge::generateMessage(int priority)
{
    char *name = (char *)("Priority =" + to_string(priority)).c_str();
    // Create message object and set source and destination field.
    greenMsg *msg = new greenMsg(name);
    msg->setPriority(priority);
    return msg;
}

void Edge::forwardMessage(cMessage *msg)
{
    simtime_t delay = truncnormal(3,1);
    sendDelayed(msg, delay, "gate$o", 0);
}

class Aggregator : public cSimpleModule
{
  private:
    cMessage * timer;
    queue <greenMsg*> first;
    queue <greenMsg*> second;
    queue <greenMsg*> third;
    int duration = 1;  //seconds
    int totalMax = 80;  // basic energy
    int greenEnergy = 10;  // green energy
    int energyForEachPacket = 10; // energy that should be used to send each packet.
  protected:
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Aggregator);

void Aggregator::initialize()
{
    timer = new cMessage("one second timer");
    scheduleAt(0.0, timer);
}

void Aggregator::handleMessage(cMessage *msg)
{
//    if(msg == timer){
//        scheduleAt(simTime() + duration, timer);
//        //forward message according to priority
//        int currentGreenEnergy =  greenEnergy;
//        int restEnergy = totalMax - greenEnergy;
//        while(currentGreenEnergy >= 0){
//            if(!first.empty()){
//                if(currentGreenEnergy - energyForEachPacket >= 0){
//                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
//                    greenMsg *tmp = first.front();
//                    first.pop();
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else if (!second.empty()){
//                if(currentGreenEnergy - energyForEachPacket >= 0){
//                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
//                    greenMsg *tmp = second.front();
//                    second.pop();
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else if (!third.empty()){
//                if(currentGreenEnergy - energyForEachPacket >= 0){
//                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
//                    greenMsg *tmp = third.front();
//                    third.pop();
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else{
//                break;
//            }
//        }
//        while(restEnergy > 0){
//            if(!first.empty()){
//                if(restEnergy - energyForEachPacket > 0){
//                    restEnergy = restEnergy - energyForEachPacket;
//                    greenMsg *tmp = first.front();
//                    first.pop();
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else{
//                break;
//            }
//        }
//    }else{
//        greenMsg *inMsg = (greenMsg *)msg;
//        switch(inMsg->getPriority()){
//            case 1: first.push(inMsg);break;
//            case 2: second.push(inMsg);break;
//            case 3: third.push(inMsg);break;
//        }
//    }
}

void Aggregator::forwardMessage(cMessage *msg)
{
//    simtime_t delay = truncnormal(3,1);
//    sendDelayed(msg, delay, "gate$o", 4);
}

class Core : public cSimpleModule
{

  protected:
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Core);

void Core::initialize()
{
}

void Core::handleMessage(cMessage *msg)
{
    delete msg;
}

void Core::forwardMessage(cMessage *msg)
{

}
