#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "greenwireless_m.h"

using namespace omnetpp;



class Edge : public cSimpleModule
{
  private:
    cMessage *outMsg = nullptr;
  protected:
    virtual greenMsg *generateMessage();
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Edge);

void Edge::initialize()
{
    outMsg = generateMessage();
    simtime_t delay = truncnormal(0,1)*3;
    sendDelayed(outMsg, delay, "gate$o", 0);
}

void Edge::handleMessage(cMessage *msg)
{
}

greenMsg *Edge::generateMessage()
{

    // Create message object and set source and destination field.
    greenMsg *msg = new greenMsg("Priority = 0");
    msg->setPriority(0);
    return msg;
}

void Edge::forwardMessage(cMessage *msg)
{

}

class Aggregator : public cSimpleModule
{
  protected:
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Aggregator);

void Aggregator::initialize()
{
}

void Aggregator::handleMessage(cMessage *msg)
{
    forwardMessage(msg);
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
