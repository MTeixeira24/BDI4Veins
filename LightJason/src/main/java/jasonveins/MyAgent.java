/*
 * @cond LICENSE
 * ######################################################################################
 * # LGPL License                                                                       #
 * #                                                                                    #
 * # This file is part of the LightJason                                                #
 * # Copyright (c) 2015-16, LightJason (info@lightjason.org)                            #
 * # This program is free software: you can redistribute it and/or modify               #
 * # it under the terms of the GNU Lesser General Public License as                     #
 * # published by the Free Software Foundation, either version 3 of the                 #
 * # License, or (at your option) any later version.                                    #
 * #                                                                                    #
 * # This program is distributed in the hope that it will be useful,                    #
 * # but WITHOUT ANY WARRANTY; without even the implied warranty of                     #
 * # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                      #
 * # GNU Lesser General Public License for more details.                                #
 * #                                                                                    #
 * # You should have received a copy of the GNU Lesser General Public License           #
 * # along with this program. If not, see http://www.gnu.org/licenses/                  #
 * ######################################################################################
 * @endcond
 */

package jasonveins;

import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.agent.IBaseAgent;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import jasonveins.environment.Lane;

import javax.annotation.Nonnull;


/**
 * agent class with annotation to mark the class that actions are inside
 */
@IAgentAction
public final class MyAgent extends IBaseAgent<MyAgent>
{

    /**
     * serial id
     */
    private static final long serialVersionUID = -2111543876806742109L;

    private int speed = 0;

    private Lane lane;

    private int waitingTime;

    private int agentId;

    /**
     * constructor of the agent
     *
     * @param p_configuration agent configuration of the agent generator
     */

    MyAgent(@Nonnull final IAgentConfiguration<MyAgent> p_configuration, @Nonnull int p_speed, Lane p_lane, int time, int id)
    {
        super( p_configuration );
        speed = p_speed;
        lane = p_lane;
        waitingTime = time;
        agentId = id;
    }

    @IAgentActionFilter
    @IAgentActionName( name = "vehicle/wait" )
    public final void tickDown(){
        waitingTime -= 1;
    }

    public final int getSpeed(){
        return this.speed;
    }
    public final int id(){return this.agentId;}
    public final Lane getLane(){
        return this.lane;
    }
    public final int getWaitingTime(){return this.waitingTime;}

    @IAgentActionFilter
    @IAgentActionName( name = "lane/move" )
    private void move()
    {
        lane.advanceVehicle(this);
    }
    @IAgentActionFilter
    @IAgentActionName( name = "lane/enter" )
    private void enterLane(){
        lane.insertVehicle(this);
    }

}
