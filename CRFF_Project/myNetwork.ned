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
simple File
{
    parameters:
        int seqNumber = default(intuniform(0, 10));
        volatile string name = default("file");		// file name	
        volatile int size @unit(b) = default(0b);  // file size in bits
}

simple Host
{
    parameters:
        double txRate @unit(bps);           // transmission rate
        double x @unit(m);                  // the x coordinate of the host
        double y @unit(m);                  // the y coordinate of the host
        double requestInterval @unit(s);
    gates:
        input in @directIn;
}

simple Server
{
    parameters:
        @display("i=device/antennatower_l");
        double x @unit(m); // the x coordinate of the server
        double y @unit(m); // the y coordinate of the server
    gates:
        input in @directIn;
}

module ComplexHost
{
    parameters:
        int maxNumFiles = default(0);		// the maximum ammout of files a host can store
        @display("i=device/pc_s");
    submodules:
        files[maxNumFiles]: File;
        host: Host;
}

network MyNetwork
{
    parameters:
        int numHosts;  // number of hosts
        @display("bgi=background/streetmap,s;bgb=1000,1000");
    submodules:
        server: Server;
        host[numHosts]: ComplexHost;
}
