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
#include "../NegotiationMessage_m.h"
}}
packet NegotiationMessage;
//Used to trade vote data, such election results and votes
packet DataExchange extends NegotiationMessage {
    int platoonId;
    int type; //Either VOTE_DATA or ELECTION_RESULT
    int data[]; //Array containing votes or election_results
}