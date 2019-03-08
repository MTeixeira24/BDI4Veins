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

package veinsbdi.traffic;
import org.car2x.veins.modules.mobility.traci.TraCIBaseTrafficManager;

simple LaneTrafficManager like TraCIBaseTrafficManager{
    parameters:
        //Should vehicle pairs be continuously injected?
        bool continuousInjection = default(false);
        //Number of vehicle pairs at simulation start
        int vPairs = default(1);
        //Number of lane pairs to use. Max value 4
        int lanePairs = default(1);   
        //How much time to wait between injections
        int injectionTimeHeadway @unit("s") = default(4s);
        //How fast are the pairs moving
        int travelSpeed @unit("kmph") = default(80kmph);
        //Whats the vehicle type to be generated?
        string vType = default("vBargain");
        //cpp class of this node
        @class(LaneTrafficManager);
}