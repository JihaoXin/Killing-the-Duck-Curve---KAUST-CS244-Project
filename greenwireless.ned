//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple Edge
{
    parameters:
        @display("i=device/pc");
        bool alive = default(false);
    gates:
        inout gate[];
}

simple Aggregator
{
    parameters:
        @display("i=misc/cloud");
    gates:
        inout gate[];
}

simple Core
{
    parameters:
        @display("i=device/server");
    gates:
        inout gate[];
}

network GreenNetwork
{
    @display("bgb=753,559");
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100ms;
        }
    submodules:
        edge1: Edge {
            @display("p=202,35");
            alive = true;
        }
        edge2: Edge {
            @display("p=290,23");
//            alive = true;
        }
        edge3: Edge {
            @display("p=376,23");
//            alive = true;
        }
        edge4: Edge {
            @display("p=436,35");
//            alive = true;
        }
        edge5: Edge {
            @display("p=556,108");
//            alive = true;
        }
        edge6: Edge {
            @display("p=589,186");
//            alive = true;
        }
        edge7: Edge {
            @display("p=611,249");
//            alive = true;
        }
        edge8: Edge {
            @display("p=556,311");
//            alive = true;
        }
        edge9: Edge {
            @display("p=391,426");
//            alive = true;
        }
        edge10: Edge {
            @display("p=308,435");
//            alive = true;
        }
        edge11: Edge {
            @display("p=236,443");
//            alive = true;
        }
        edge12: Edge {
            @display("p=169,426");
            
        }
        edge13: Edge {
            @display("p=75,311");
        }
        edge14: Edge {
            @display("p=42,225");
        }
        edge15: Edge {
            @display("p=33,155");
        }
        edge16: Edge {
            @display("p=60,83");
        }
        aggregator1: Aggregator {
            @display("p=311,128");
        }
        aggregator2: Aggregator {
            @display("p=469,209");
        }
        aggregator3: Aggregator {
            @display("p=272,346");
        }
        aggregator4: Aggregator {
            @display("p=140,209");
        }
        core: Core {
            @display("p=308,236");
        }
    connections:
        edge1.gate++ <--> Channel <--> aggregator1.gate++;
        edge2.gate++ <--> Channel <--> aggregator1.gate++;
        edge3.gate++ <--> Channel <--> aggregator1.gate++;
        edge4.gate++ <--> Channel <--> aggregator1.gate++;
        edge5.gate++ <--> Channel <--> aggregator2.gate++;
        edge6.gate++ <--> Channel <--> aggregator2.gate++;
        edge7.gate++ <--> Channel <--> aggregator2.gate++;
        edge8.gate++ <--> Channel <--> aggregator2.gate++;
        edge9.gate++ <--> Channel <--> aggregator3.gate++;
        edge10.gate++ <--> Channel <--> aggregator3.gate++;
        edge11.gate++ <--> Channel <--> aggregator3.gate++;
        edge12.gate++ <--> Channel <--> aggregator3.gate++;
        edge13.gate++ <--> Channel <--> aggregator4.gate++;
        edge14.gate++ <--> Channel <--> aggregator4.gate++;
        edge15.gate++ <--> Channel <--> aggregator4.gate++;
        edge16.gate++ <--> Channel <--> aggregator4.gate++;
        aggregator1.gate++ <--> Channel <--> core.gate++;
        aggregator2.gate++ <--> Channel <--> core.gate++;
        aggregator3.gate++ <--> Channel <--> core.gate++;
        aggregator4.gate++ <--> Channel <--> core.gate++;
}

