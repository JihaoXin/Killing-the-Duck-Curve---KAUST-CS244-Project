#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "greenwireless_m.h"
#include <Windows.h>
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
    int duration = 1;  //seconds
    int newPacketInEachTick = 5; // new packets that should be generated in each tick
    int forwardPacketNum = 5; // forwarding packets in each tick
    int basicEnergy = 800;  // basic energy
    int greenEnergy = 250;  // green energy
    int energyForEachPacket = 1; // energy that should be used to send each packet.
    int time = 0;
    mutex g_mutex;
    cHistogram onePacketNum;
    cHistogram twoPacketNum;
    cHistogram threePacketNum;
    cOutVector greenEnergyVector;
    cOutVector totalEnergyVector;
    cOutVector dirtyEnergyVector;
    cOutVector restEnergyVector;
    int onePacketCount;
    int twoPacketCount;
    int threePacketCount;
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
    onePacketCount = 0;
    twoPacketCount = 0;
    threePacketCount = 0;
    greenEnergyVector.setName("green");
    totalEnergyVector.setName("total");
    dirtyEnergyVector.setName("dirty");
    restEnergyVector.setName("rest");
    if (par("alive").boolValue() == true){
        timer = new cMessage("one second timer");
        scheduleAt(0.0, timer);
    }
    WATCH(onePacketCount);
    WATCH(twoPacketCount);
    WATCH(threePacketCount);
}
double Edge::normal(double x, double miu, double sigma) {
    double PI = 3.14;
    return 1.0/(sqrt(2*PI)*sigma) * exp(-1*(x-miu)*(x-miu)/(2*sigma*sigma));
}
void Edge::handleMessage(cMessage *msg)
{
    if (msg == timer){
        queue <greenMsg*> first;
        queue <greenMsg*> second;
        queue <greenMsg*> third;
        time++;
        int newPacketInEachTick = 300 + rand() % 60;
        int additionP2 = rand()%40;
        scheduleAt(simTime() + duration, timer);
        for(int i = 0; i < newPacketInEachTick; i++){
            greenMsg *outMsg;
            if(i<280+rand()%20){
                outMsg = generateMessage(1);
            }else{

                if((280+rand()%20<=i)&&(i<290+rand()%200)){
                    EV << "????????????????";
                    outMsg = generateMessage(2);
                }else{
                    if(additionP2>0){
                        outMsg = generateMessage(2);
                        additionP2--;
                    }else{
                        outMsg = generateMessage(3);
                    }
                }
            }
//            EV<<outMsg->getPriority();
            switch(outMsg->getPriority()){
                case 1: first.push(outMsg);break;
                case 2: second.push(outMsg);break;
                case 3: third.push(outMsg);break;
            }
        }
 //================================================ our strategy ============================================
//        //TODO: queue thread security??
////        default_random_engine generator;
////        default_random_engine generator;
//        int currentGreenEnergy =  (int)(normal(time, 12*3600, 10000.0)*greenEnergy*40000) + 100;
//        int netEnergyUsed = 0;
//        int dirtyEnergy = 0;
//        greenEnergyVector.record(currentGreenEnergy);
////        EV << currentGreenEnergy;
//        int restEnergy = basicEnergy - currentGreenEnergy;
//        EV << restEnergy;
//        while(currentGreenEnergy >= 0){
//            if(!first.empty()){
//                if(currentGreenEnergy - energyForEachPacket >= 0){
//                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
//                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
//                    greenMsg *tmp = first.front();
//                    first.pop();
//                    onePacketCount++;
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else if (!second.empty()){
//                if(currentGreenEnergy - energyForEachPacket >= 0){
//                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
//                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
//                    greenMsg *tmp = second.front();
//                    second.pop();
//                    twoPacketCount ++;
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else if (!third.empty()){
//                if(currentGreenEnergy - energyForEachPacket >= 0){
//                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
//                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
//                    greenMsg *tmp = third.front();
//                    third.pop();
//                    threePacketCount ++;
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else{
//                break;
//            }
//        }
////        EV << restEnergy;
//        while(restEnergy >= 0){
//            if(!first.empty()){
//                if(restEnergy - energyForEachPacket >= 0){
//                    restEnergy = restEnergy - energyForEachPacket;
//                    netEnergyUsed = netEnergyUsed + energyForEachPacket;
//                    dirtyEnergy = dirtyEnergy + energyForEachPacket;
//                    greenMsg *tmp = first.front();
//                    first.pop();
//                    onePacketCount ++;
//                    forwardMessage(tmp);
//                }else{
//                    break;
//                }
//            }else{
//                break;
//            }
//        }
////        restEnergyVector.record(restEnergy);
//        dirtyEnergyVector.record(dirtyEnergy);
//        totalEnergyVector.record(netEnergyUsed);
//============================================= normal strategy ============================================
                int currentGreenEnergy =  (int)(normal(time, 12*3600, 10000.0)*greenEnergy*40000) + 100;
                int netEnergyUsed = 0;
                int dirtyEnergy = 0;
                greenEnergyVector.record(currentGreenEnergy);
                int restEnergy = basicEnergy - currentGreenEnergy;
                EV << restEnergy;
                while(currentGreenEnergy >= 0){
                    if(!first.empty()){
                        if(currentGreenEnergy - energyForEachPacket >= 0){
                            currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
                            netEnergyUsed = netEnergyUsed + energyForEachPacket;
                            greenMsg *tmp = first.front();
                            first.pop();
                            onePacketCount++;
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
                            twoPacketCount ++;
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
                            threePacketCount ++;
                            forwardMessage(tmp);
                        }else{
                            break;
                        }
                    }else{
                        break;
                    }
                }
        //        EV << restEnergy;
                while(restEnergy >= 0){
                    if(!first.empty()){
                        if(restEnergy - energyForEachPacket >= 0){
                            restEnergy = restEnergy - energyForEachPacket;
                            netEnergyUsed = netEnergyUsed + energyForEachPacket;
                            dirtyEnergy = dirtyEnergy + energyForEachPacket;
                            greenMsg *tmp = first.front();
                            first.pop();
                            onePacketCount ++;
                            forwardMessage(tmp);
                        }else{
                            break;
                        }
                    }else if (!second.empty()){
                        if(restEnergy - energyForEachPacket >= 0){
                            restEnergy = restEnergy - energyForEachPacket;
                            netEnergyUsed = netEnergyUsed + energyForEachPacket;
                            dirtyEnergy = dirtyEnergy + energyForEachPacket;
                            greenMsg *tmp = second.front();
                            second.pop();
                            twoPacketCount ++;
                            forwardMessage(tmp);
                        }else{
                            break;
                        }
                    }else if (!third.empty()){
                        if(restEnergy - energyForEachPacket >= 0){
                            restEnergy = restEnergy - energyForEachPacket;
                            netEnergyUsed = netEnergyUsed + energyForEachPacket;
                            dirtyEnergy = dirtyEnergy + energyForEachPacket;
                            greenMsg *tmp = third.front();
                            third.pop();
                            threePacketCount ++;
                            forwardMessage(tmp);
                        }else{
                            break;
                        }
                    }else{
                        break;
                    }
                }
        //        restEnergyVector.record(restEnergy);
                dirtyEnergyVector.record(dirtyEnergy);
                totalEnergyVector.record(netEnergyUsed);



    }
}

greenMsg *Edge::generateMessage(int priority)
{
//    int priority = 1 + rand() % 3;
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
    int basicEnergy = 80;  // basic energy
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
    if(msg == timer){
        scheduleAt(simTime() + duration, timer);
        //forward message according to priority
        int currentGreenEnergy =  greenEnergy;
        int restEnergy = basicEnergy - greenEnergy;
        while(currentGreenEnergy >= 0){
            if(!first.empty()){
                if(currentGreenEnergy - energyForEachPacket >= 0){
                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
                    greenMsg *tmp = first.front();
                    first.pop();
                    forwardMessage(tmp);
                }else{
                    break;
                }
            }else if (!second.empty()){
                if(currentGreenEnergy - energyForEachPacket >= 0){
                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
                    greenMsg *tmp = second.front();
                    second.pop();
                    forwardMessage(tmp);
                }else{
                    break;
                }
            }else if (!third.empty()){
                if(currentGreenEnergy - energyForEachPacket >= 0){
                    currentGreenEnergy = currentGreenEnergy - energyForEachPacket;
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
    }else{
        greenMsg *inMsg = (greenMsg *)msg;
        switch(inMsg->getPriority()){
            case 1: first.push(inMsg);break;
            case 2: second.push(inMsg);break;
            case 3: third.push(inMsg);break;
        }
    }
}

void Aggregator::forwardMessage(cMessage *msg)
{
    simtime_t delay = truncnormal(3,1);
    sendDelayed(msg, delay, "gate$o", 4);
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
