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

package jasonveins.omnet;

import jasonveins.omnet.decision.InstructionModel;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.agent.IBaseAgent;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;

import javax.annotation.Nonnull;


/**
 * agent class with annotation to mark the class that actions are inside
 */
@IAgentAction
public final class ManagerAgent extends IBaseAgent<ManagerAgent>
{
    private int id;
    private AgentManager agentManager;

    /**
     * serial id
     */
    private static final long serialVersionUID = -2111893876806742109L;


    /**
     * constructor of the agent
     *
     * @param p_configuration agent configuration of the agent generator
     */

    ManagerAgent(@Nonnull final IAgentConfiguration<ManagerAgent> p_configuration, @Nonnull AgentManager m_am,@Nonnull final int m_id)
    {
        super( p_configuration );
        id = m_id;
        agentManager = m_am;
    }

    public int id(){
        return this.id;
    }

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/self/slow" )
    private void transmitDecision()
    {
        InstructionModel iOb = new InstructionModel(this.id, 4);
        iOb.pushDouble(0.5);
        agentManager.addInstruction(iOb);
    }

}
