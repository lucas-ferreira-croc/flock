#ifndef CONNECTION_HPP
#define CONNECTION_HPP


// should we have connected?
struct Connection
{
    int _fromNodeId;
    int _toNodeId;

    Connection(int from, int to) : _fromNodeId(from), _toNodeId(to) {}
};

#endif