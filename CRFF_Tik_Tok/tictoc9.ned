//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Tic9
{
    parameters:
        @display("i=block/routing");
        volatile double delayC @unit(s);
        volatile double delayD @unit(s);
        volatile double delayED @unit(s);
    gates:
        input in;
        output out;
}

simple Toc9
{
    parameters:
        @display("i=block/process");
    gates:
        input in;
        output out;
}

network Tictoc9
{
    submodules:
        tic: Tic9 {
            parameters:
                @display("i=,cyan");
        }
        toc: Toc9 {
            parameters:
                @display("i=,gold");
        }
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}
