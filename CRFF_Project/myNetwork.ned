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

simple Host
{
    parameters:
        @display("i=block/routing");
    gates:
        inout gate[];
}

network MyNetwork
{
    types:
        channel Channel extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        clients[6]: Host;
    connections:
        clients[0].gate++ <--> Channel <--> clients[1].gate++;
        clients[1].gate++ <--> Channel <--> clients[2].gate++;
        clients[1].gate++ <--> Channel <--> clients[4].gate++;
        clients[3].gate++ <--> Channel <--> clients[4].gate++;
        clients[4].gate++ <--> Channel <--> clients[5].gate++;
}