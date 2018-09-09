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

simple JoinBDITrafficManager like TraCIBaseTrafficManager {

    parameters:
        //number of vehicles per platoon.
        double platoonSize = default(4);
        //number of platoons to inject 
        int nPlatoons = default(1);
        //number of lanes available
        int nLanes;
        //platoon insertion speed. can be a random distribution
        volatile double platoonInsertSpeed @unit("kmph");
        //sumo vehicle type for platooning cars
        string platooningVType;
        //insert distance and headway. distance is computed as:
        //dist = insertHeadway * insertSpeed + insertDistance
        //default values are for PATH's CACC
        double platoonInsertDistance @unit("m") = default(5m);
        double platoonInsertHeadway @unit("s") = default(0s);
        double platoonLeaderHeadway @unit("s") = default(1.2s);
        //lane in which to insert the joiner. -1 To not insert
        int joinerLane = default(-1);
        @class(JoinBDITrafficManager);
}