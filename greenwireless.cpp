#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Edge : public cSimpleModule
{
  protected:
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Edge);

void Edge::initialize()
{

}

void Edge::handleMessage(cMessage *msg)
{

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

}

void Aggregator::forwardMessage(cMessage *msg)
{

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

}

void Core::forwardMessage(cMessage *msg)
{

}
