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

//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2004 Andras Varga
// Updated by Alexander Senior, 2011.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
// $Id: tictoc13.ned,v 1.1 2005/09/28 23:58:04 ahmet Exp $

simple Txc14 
{
    parameters:
        int route0;
        int route1;
        int route2;
        int route3;
        int route4;
        int route5;
        int route6;
        int route7;
        int route8;
        int route9;
        @display("i=block/routing");
        
    gates:
        input in[];
        output out[];
}

network Tictoc14
{
    types:
        channel Channel extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        tic[10]: Txc14;
    connections:
        tic[0].out++ --> Channel --> tic[1].in++;
        tic[0].in++ <-- Channel <-- tic[1].out++;
        
        tic[0].out++ --> Channel --> tic[2].in++;
        tic[0].in++ <-- Channel <-- tic[2].out++;
        
        tic[0].out++ --> Channel --> tic[5].in++;
        tic[0].in++ <-- Channel <-- tic[5].out++;

        tic[1].out++ --> Channel --> tic[2].in++;
        tic[1].in++ <-- Channel <-- tic[2].out++;

        tic[1].out++ --> Channel --> tic[3].in++;
        tic[1].in++ <-- Channel <-- tic[3].out++;

        tic[2].out++ --> Channel --> tic[3].in++;
        tic[2].in++ <-- Channel <-- tic[3].out++;

        tic[2].out++ --> Channel --> tic[5].in++;
        tic[2].in++ <-- Channel <-- tic[5].out++;

        tic[2].out++ --> Channel --> tic[4].in++;
        tic[2].in++ <-- Channel <-- tic[4].out++;

        tic[3].out++ --> Channel --> tic[4].in++;
        tic[3].in++ <-- Channel <-- tic[4].out++;

        tic[4].out++ --> Channel --> tic[6].in++;
        tic[4].in++ <-- Channel <-- tic[6].out++;

        tic[4].out++ --> Channel --> tic[8].in++;
        tic[4].in++ <-- Channel <-- tic[8].out++;

        tic[5].out++ --> Channel --> tic[6].in++;
        tic[5].in++ <-- Channel <-- tic[6].out++;

        tic[5].out++ --> Channel --> tic[7].in++;
        tic[5].in++ <-- Channel <-- tic[7].out++;

        tic[6].out++ --> Channel --> tic[7].in++;
        tic[6].in++ <-- Channel <-- tic[7].out++;

        tic[6].out++ --> Channel --> tic[8].in++;
        tic[6].in++ <-- Channel <-- tic[8].out++;

        tic[6].out++ --> Channel --> tic[9].in++;
        tic[6].in++ <-- Channel <-- tic[9].out++;

        tic[7].out++ --> Channel --> tic[9].in++;
        tic[7].in++ <-- Channel <-- tic[9].out++;

        tic[8].out++ --> Channel --> tic[9].in++;
        tic[8].in++ <-- Channel <-- tic[9].out++;
}