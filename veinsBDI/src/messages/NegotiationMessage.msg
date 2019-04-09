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

cplusplus {{
    /** message type for maneuver messages */
    #include <unordered_set>
    typedef std::unordered_set<int> IntSet;
    static const int NEGOTIATION_TYPE = 22347;
}}
class noncobject IntSet;
// General message to initiate negotiation procedures.
// Only children of this message should be initialized.
packet NegotiationMessage {
    // id of the originator of this message
    int vehicleId;
    // id of the destination of this message
    int destinationId;
    // sumo external id of the sender
    string externalId;
    //Platoon ID of sender
    int platoonId;
    //Set of target nodes
    IntSet targets;
    //Is this message meant for the entire platoon or a select few?
    bool forWholePlatoon;
    //Identifier of the message
    int messageId;
    //Identifier of the message it is replying to
    int replyMessageId;
    //Specifies what type of message this is. e.g. BID, ACK, OK, etc..
    int messageType;
    //Specifies if this an ack type message
    bool isAck=false;
}